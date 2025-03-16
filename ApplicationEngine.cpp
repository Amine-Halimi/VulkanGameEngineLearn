#include "ApplicationEngine.hpp"
#include "SimpleRenderingSystem.hpp"
#include "keyboardController.hpp"
#include "mouseController.hpp"

//std
#include "stdexcept"
#include "array"
#include "chrono"

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
		
		auto currentTime = std::chrono::high_resolution_clock::now();

		auto cameraObject = weEngineGameObject::createGameObject();
		KeyboardMovementController cameraController{};
		MouseMovementController mouseController{};

		while (!weEngineWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			float screenAspectRatio = weEngineRenderer.getAspectRatio();

			cameraController.moveInPlaceXZ(weEngineWindow.getGLFWwindow(), frameTime, cameraObject);
			mouseController.processMouseMovement(weEngineWindow.getGLFWwindow(), cameraObject);


			camera.setViewYXZ(cameraObject.transformComp.translation, cameraObject.transformComp.rotation);
			
			camera.setPerspectiveProjection(glm::radians(50.0f), screenAspectRatio, 0.1f, 100.0f);
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
		std::shared_ptr<weEngineModel> weEngineModel = weEngineModel::createModelFromFile(weEngineDevice, "models\\backpack\\backpack.obj");

		auto gameObj = weEngineGameObject::createGameObject();

		gameObj.model = weEngineModel;
		gameObj.transformComp.translation = { 0.0f, 0.0f, 2.5f };
		gameObj.transformComp.scale = { 1.0f, 1.0f, 1.0f };
		
		gameObjects.push_back(std::move(gameObj));
	}




}