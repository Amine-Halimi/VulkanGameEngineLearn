#pragma once
#pragma once

#include "weEnginePipeline.hpp"
#include "weEngineGameObject.hpp"
#include "weEngineDevice.hpp"

//std
#include "memory"
#include "vector"

/*
*
* SimpleRenderingSystem is a class that implements the graphics pipeline and rendering the in-game objects
*
* author: Amine Halimi
*/

namespace weEngine {
	class SimpleRenderingSystem
	{
	public:
		SimpleRenderingSystem(weEngineDevice& device, VkRenderPass renderPass);
		~SimpleRenderingSystem();

		SimpleRenderingSystem(const SimpleRenderingSystem&) = delete;
		SimpleRenderingSystem& operator=(const SimpleRenderingSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<weEngineGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		
		weEngineDevice& weEngineDevice;
		std::unique_ptr<weEnginePipeline> weEnginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}