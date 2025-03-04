#pragma once

#include "weEngineWindow.hpp"
#include "weEnginePipeline.hpp"
#include "weEngineDevice.hpp"
#include "weEngineSwapChain.hpp"
#include "weEngineModel.hpp"

//std
#include "memory"
#include "vector"
/*
*
* ApplicationEngine is a class for running the vulkan instance.
*
* author: Amine Halimi
*/

namespace weEngine {
	class ApplicationEngine
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		ApplicationEngine();
		~ApplicationEngine();

		ApplicationEngine(const ApplicationEngine&) = delete;
		ApplicationEngine& operator=(const ApplicationEngine&) = delete;

		void run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void loadModels();


		weEngineWindow weEngineWindow{ WIDTH, HEIGHT, "Hello from Vulkan" };
		weEngineDevice weEngineDevice{ weEngineWindow };
		weEngineSwapChain weEngineSwapChain{ weEngineDevice, weEngineWindow.getExtent()};
		std::unique_ptr<weEnginePipeline> weEnginePipeline;

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<weEngineModel> weEngineModel;
	};
}