#include "ApplicationEngine.hpp"
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

	ApplicationEngine::ApplicationEngine()
	{
		loadGameObjects();
	}

	ApplicationEngine::~ApplicationEngine()
	{
	}

	/*
	* Main loop of the applcation engine
	*/
	void ApplicationEngine::run()
	{
		SimpleRenderingSystem renderSystem{ weEngineDevice, weEngineRenderer.getSwapChainRenderPass() };
		while (!weEngineWindow.shouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = weEngineRenderer.beginFrame())
			{
				weEngineRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, gameObjects);
				weEngineRenderer.endSwapChainRenderPass(commandBuffer);
				weEngineRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(weEngineDevice.device()); //Wait for the GPU to finish its operation before closing
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



}