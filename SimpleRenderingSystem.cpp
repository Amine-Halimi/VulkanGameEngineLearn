#include "SimpleRenderingSystem.hpp"

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

	SimpleRenderingSystem::SimpleRenderingSystem(weEngine::weEngineDevice& device, VkRenderPass renderPass): weEngineDevice(device)
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderingSystem::~SimpleRenderingSystem()
	{
		vkDestroyPipelineLayout(weEngineDevice.device(), pipelineLayout, nullptr);
	}


	/*
	* Creates a pipeline layout for the weEnginePipeline object
	*/

	void SimpleRenderingSystem::createPipelineLayout()
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
	void SimpleRenderingSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before create the pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		weEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		weEnginePipeline = make_unique<weEngine::weEnginePipeline>(
			weEngineDevice,
			"shaders\\simpleVertexShader.vert.spv",
			"shaders\\simpleFragmentShader.frag.spv",
			pipelineConfig);
	}

	void SimpleRenderingSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<weEngineGameObject>& gameObjects)
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