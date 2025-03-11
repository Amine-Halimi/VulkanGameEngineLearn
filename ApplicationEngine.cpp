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
		createPipeline();
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
			
			if (auto commandBuffer = weEngineRenderer.beginFrame())
			{
				weEngineRenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
				weEngineRenderer.endSwapChainRenderPass(commandBuffer);
				weEngineRenderer.endFrame();
			}
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
		assert(pipelineLayout != nullptr && "Cannot create pipeline before create the pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		weEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		pipelineConfig.renderPass = weEngineRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		weEnginePipeline = make_unique<weEngine::weEnginePipeline>(
			weEngineDevice,
			"shaders\\simpleVertexShader.vert.spv",
			"shaders\\simpleFragmentShader.frag.spv",
			pipelineConfig);
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

}