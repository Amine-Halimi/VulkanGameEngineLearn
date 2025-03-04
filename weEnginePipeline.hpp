#pragma once
#include "weEngineDevice.hpp"

#include "string"
#include "vector"


/*
* 
* Pipeline engine for the application
* 
*/
namespace weEngine
{
	struct PipelineConfigInfo
	{
		//Deleting copy constructor
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	/*
	* 
	* weEnginePipeline handles the graphics pipeline of the engine.
	* 
	*/
	class weEnginePipeline
	{
	public:
		weEnginePipeline(
			weEngineDevice &device,
			const std::string vertexPath,
			const std::string fragPath,
			const PipelineConfigInfo &configInfo);
		~weEnginePipeline();
		
		void bind(VkCommandBuffer commandBuffer);
		//avoiding copying pointers by accident
		weEnginePipeline(const weEnginePipeline&) = delete;
		weEnginePipeline operator=(const weEnginePipeline&) = delete;

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string &filepath);


		void createGraphicsPipeline
			(const std::string vertexPath,
			const std::string fragPath,
			const PipelineConfigInfo &configInfo);

		void createShaderModule(std::vector<char>& code, VkShaderModule* shaderModule);

		
		weEngineDevice& weEngineDevice;

		//Vulkan types
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

	};
}