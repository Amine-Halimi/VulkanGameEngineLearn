#include "ApplicationEngine.hpp"

//std
#include "stdexcept"
#include "array"

//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

using std::make_unique;


namespace weEngine
{
	struct SimplePushConstantData {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	ApplicationEngine::ApplicationEngine()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	ApplicationEngine::~ApplicationEngine()
	{
		vkDestroyPipelineLayout(weEngineDevice.device(), pipelineLayout, nullptr);
	}

	/*
	* Creates the game objects for the app.
	*/
	void ApplicationEngine::loadGameObjects()
	{
		std::vector<weEngineModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto weEngineModel = std::make_shared<weEngine::weEngineModel>(weEngineDevice, vertices);

		auto triangle = weEngineGameObject::createGameObject();
		triangle.model = weEngineModel;
		triangle.color = { 0.0f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.0f, 0.5f };
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
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
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //Set the push constant range for the vertex and fragment stage.
		pushConstantRange.size = sizeof(SimplePushConstantData);
		pushConstantRange.offset = 0; //offset if we need to separate the data for vertex and fragment stages


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Empty layout
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1; // Preliminary data sent to the shader
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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
		assert(weEngineSwapChain != nullptr && "Cannot create pipeline before creating the swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before create the pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		weEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		pipelineConfig.renderPass = weEngineSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		weEnginePipeline = make_unique<weEngine::weEnginePipeline>(
			weEngineDevice,
			"shaders\\simpleVertexShader.vert.spv",
			"shaders\\simpleFragmentShader.frag.spv",
			pipelineConfig);
	}

	/*
	* Waits for the window to end resizing and recreates the swap chain
	*/
	void ApplicationEngine::recreateSwapChain()
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
			weEngineSwapChain = std::make_unique<weEngine::weEngineSwapChain>(weEngineDevice, extent, std::move(weEngineSwapChain));
			if (weEngineSwapChain->imageCount() != commandBuffers.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		

		//If the render passes are compatible, no need to recreate the pipeline
		createPipeline();
	}

	/*
	* Creates command buffers which hold the vulkan command drawing the fragments.
	* There are two command buffers made, each for the framebuffer
	*/
	void ApplicationEngine::createCommandBuffers()
	{
		commandBuffers.resize(weEngineSwapChain->imageCount());
		
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
	void ApplicationEngine::freeCommandBuffers()
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
	* Adds commands to the command buffer in the given index
	*/
	void ApplicationEngine::recordCommandBuffer(int imageIndex)
	{

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = weEngineSwapChain->getRenderPass();
		renderPassInfo.framebuffer = weEngineSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = weEngineSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; //Color buffer
		clearValues[1].depthStencil = { 1.0f, 0 }; //Depth buffer

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Add a dynamically created viewport+scissor
		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<uint32_t>(weEngineSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<uint32_t>(weEngineSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = { {0, 0}, weEngineSwapChain->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}
	}

	void ApplicationEngine::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		weEnginePipeline->bind(commandBuffer);

		for (auto& gameObj : gameObjects)
		{
			gameObj.transform2d.rotation = glm::mod(gameObj.transform2d.rotation + 0.01f, glm::two_pi<float>());
			SimplePushConstantData pushData{};
			pushData.offset = gameObj.transform2d.translation;
			pushData.color = gameObj.color;
			pushData.transform = gameObj.transform2d.mat2();

			vkCmdPushConstants(commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&pushData);

			gameObj.model->bind(commandBuffer);
			gameObj.model->draw(commandBuffer);

		}
	}

	/*
	* Draws the frame and acquires the next one.
	*/
	void ApplicationEngine::drawFrame()
	{
		uint32_t imageIndex;
		auto result = weEngineSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = weEngineSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || weEngineWindow.wasWindowResized())
		{
			weEngineWindow.resetWindowResizedFlags();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}


}