#include "ApplicationEngine.hpp"

//std
#include "stdexcept"
#include "array"

using std::make_unique;

namespace weEngine
{
	ApplicationEngine::ApplicationEngine()
	{
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	ApplicationEngine::~ApplicationEngine()
	{
		vkDestroyPipelineLayout(weEngineDevice.device(), pipelineLayout, nullptr);
	}
	/*
	* Main loop of the applcation engine
	*/
	void ApplicationEngine::run()
	{
		while (!weEngineWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(weEngineDevice.device()); //Wait for the GPU to finish its operation before closing
	}

	/*
	* Creates a pipeline layout for the weEnginePipeline object
	*/

	void ApplicationEngine::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Empty layout
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Preliminary data sent to the shader
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(weEngineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}
	/*
	* Creates the weEnginePipeline object
	*/
	void ApplicationEngine::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		weEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig,
			weEngineSwapChain.width(),
			weEngineSwapChain.height()
		);
		pipelineConfig.renderPass = weEngineSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		weEnginePipeline = make_unique<weEngine::weEnginePipeline>(
			weEngineDevice,
			"shaders\\simpleVertexShader.vert.spv",
			"shaders\\simpleFragmentShader.frag.spv",
			pipelineConfig);
	}

	/*
	* Creates command buffers which hold the vulkan command drawing the fragments.
	* There are two command buffers made, each for the framebuffer
	*/
	void ApplicationEngine::createCommandBuffers()
	{
		commandBuffers.resize(weEngineSwapChain.imageCount());
		
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = weEngineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(weEngineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate command buffers layout");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = weEngineSwapChain.getRenderPass();
			renderPassInfo.framebuffer = weEngineSwapChain.getFrameBuffer(i);
			
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = weEngineSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; //Color buffer
			clearValues[1].depthStencil = { 1.0f, 0 }; //Depth buffer

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			weEnginePipeline->bind(commandBuffers[i]);
			weEngineModel->bind(commandBuffers[i]);
			weEngineModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to end recording command buffer");
			}
		}
	}

	/*
	* Draws the frame and acquires the next one.
	*/
	void ApplicationEngine::drawFrame()
	{
		uint32_t imageIndex;
		auto result = weEngineSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		result = weEngineSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}

	/*
	* Creates the engine model
	*/
	void ApplicationEngine::loadModels()
	{
		std::vector<weEngineModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		weEngineModel = std::make_unique<weEngine::weEngineModel>(weEngineDevice, vertices);
	}
}