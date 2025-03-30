#pragma once

#include "weEnginePipeline.hpp"
#include "weEngineGameObject.hpp"
#include "weEngineDevice.hpp"
#include "weEngineCamera.hpp"
#include "weEngineFrameInfo.hpp"

//std
#include "memory"
#include "vector"

/*
*
* PointLightSystem is a class that implements the point light system in the scene
*
* author: Amine Halimi
*/

namespace weEngine {
	class PointLightSystem
	{
	public:
		PointLightSystem(weEngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		
		weEngineDevice& weEngineDevice;
		std::unique_ptr<weEnginePipeline> weEnginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}