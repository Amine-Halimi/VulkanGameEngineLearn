#include "weEngineRenderer.hpp"

//std
#include "stdexcept"
#include "array"
#include "cassert"

using std::make_unique;


namespace weEngine
{


	weEngineRenderer::weEngineRenderer(weEngine::weEngineWindow& window, weEngine::weEngineDevice& device): weEngineWindow{window}, weEngineDevice{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	weEngineRenderer::~weEngineRenderer()
	{
		freeCommandBuffers();
	}


	/*
	* Waits for the window to end resizing and recreates the swap chain
	*/
	void weEngineRenderer::recreateSwapChain()
	{
		auto extent = weEngineWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = weEngineWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(weEngineDevice.device());

		if (weEngineSwapChain == nullptr)
		{
			weEngineSwapChain = std::make_unique<weEngine::weEngineSwapChain>(weEngineDevice, extent);
		}
		else
		{
			std::shared_ptr<weEngine::weEngineSwapChain> oldSwapChain = std::move(weEngineSwapChain);
			weEngineSwapChain = std::make_unique<weEngine::weEngineSwapChain>(weEngineDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*weEngineSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image for color (or depth) format has changed.");
			}
			
		}


		//If the render passes are compatible, no need to recreate the pipeline
		//createPipeline();
	}

	/*
	* Creates command buffers which hold the vulkan command drawing the fragments.
	* There are two command buffers made, each for the framebuffer
	*/
	void weEngineRenderer::createCommandBuffers()
	{
		commandBuffers.resize(weEngineSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = weEngineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(weEngineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers layout");
		}

	}
	/*
	* Frees up the command buffer memories
	*/
	void weEngineRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			weEngineDevice.device(),
			weEngineDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}
	
	/*
	* function that begins the frame to be drawn. Then creates a command buffer
	*/
	VkCommandBuffer weEngineRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while frame is in progress.");

		auto result = weEngineSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}
	/*
	* Function ends the drawing of the frame and finishes the command buffer
	*/
	void weEngineRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame function while the frame is not in progress.");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}

		auto result = weEngineSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || weEngineWindow.wasWindowResized())
		{
			weEngineWindow.resetWindowResizedFlags();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % weEngineSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	/*
	* Starts the rendering pass of the swap chain
	*/
	void weEngineRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass function while the frame is not in progress.");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't call beginSwapChainRenderPass function with a command buffer from a different frame.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = weEngineSwapChain->getRenderPass();
		renderPassInfo.framebuffer = weEngineSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = weEngineSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; //Color buffer
		clearValues[1].depthStencil = { 1.0f, 0 }; //Depth buffer

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Add a dynamically created viewport+scissor
		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<uint32_t>(weEngineSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<uint32_t>(weEngineSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = { {0, 0}, weEngineSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void weEngineRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass function while the frame is not in progress.");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't call endSwapChainRenderPass function with a command buffer from a different frame.");

		vkCmdEndRenderPass(commandBuffer);
	}


}