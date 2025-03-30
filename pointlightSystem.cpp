#include "pointlightSystem.hpp"

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

	PointLightSystem::PointLightSystem(weEngine::weEngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout): weEngineDevice(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(weEngineDevice.device(), pipelineLayout, nullptr);
	}


	/*
	* Creates a pipeline layout for the weEnginePipeline object
	*/
	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //Set the push constant range for the vertex and fragment stage.
		//pushConstantRange.size = sizeof(SimplePushConstantData);
		//pushConstantRange.offset = 0; //offset if we need to separate the data for vertex and fragment stages

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()); // Empty layout
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before create the pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		weEnginePipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);
		
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		weEnginePipeline = make_unique<weEngine::weEnginePipeline>(
			weEngineDevice,
			"shaders\\pointLightVertexShader.vert.spv",
			"shaders\\pointLightFragmentShader.frag.spv",
			pipelineConfig);
	}
	/*
	* Renders the game objects
	*/
	void PointLightSystem::render(FrameInfo& frameInfo)
	{
		weEnginePipeline->bind(frameInfo.commandBuffer);
		
		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}

}