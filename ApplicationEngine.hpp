#pragma once

#include "weEngineWindow.hpp"
#include "weEngineGameObject.hpp"
#include "weEngineDevice.hpp"
#include "weEngineRenderer.hpp"

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
		void loadGameObjects();

		weEngineWindow weEngineWindow{ WIDTH, HEIGHT, "Hello from Vulkan" };
		weEngineDevice weEngineDevice{ weEngineWindow };
		weEngineRenderer weEngineRenderer{weEngineWindow, weEngineDevice};
		std::vector<weEngineGameObject> gameObjects;
	};
}