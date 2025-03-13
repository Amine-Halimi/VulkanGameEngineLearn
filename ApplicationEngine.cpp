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

	std::unique_ptr<weEngineModel> createCubeModel(weEngineDevice& device, glm::vec3 offset);

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
		weEngineCamera camera{};
		
		while (!weEngineWindow.shouldClose())
		{
			glfwPollEvents();
			float screenAspectRatio = weEngineRenderer.getAspectRatio();

			//camera.setViewDirection(glm::vec3{0.0f}, glm::vec3{-0.5f, 0.0f, 1.0f});
			camera.setViewTarget(glm::vec3{ -1.0f, -2.0f, 2.0f }, gameObjects[0].transformComp.translation);
			
			//camera.setOrthographicProjection(-screenAspectRatio, screenAspectRatio, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.0f), screenAspectRatio, 0.1f, 10.0f);
			if (auto commandBuffer = weEngineRenderer.beginFrame())
			{
				weEngineRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
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
		std::shared_ptr<weEngineModel> weEngineModel = createCubeModel(weEngineDevice, { 0.0f, 0.0f, 0.0f });

		auto cube = weEngineGameObject::createGameObject();

		cube.model = weEngineModel;
		cube.transformComp.translation = { 0.0f, 0.0f, 2.5f };
		cube.transformComp.scale = { 0.5f, 0.5f, 0.5f };
		
		gameObjects.push_back(std::move(cube));
	}


	std::unique_ptr<weEngineModel> createCubeModel(weEngineDevice& device, glm::vec3 offset) {
		std::vector<weEngineModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) 
		{
			v.position += offset;
		}
		return std::make_unique<weEngineModel>(device, vertices);
	}


}