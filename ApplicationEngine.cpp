#include "ApplicationEngine.hpp"
#include "SimpleRenderingSystem.hpp"
#include "keyboardController.hpp"
#include "mouseController.hpp"
#include "weEngineBuffer.hpp"
#include "weEngineFrameInfo.hpp"

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
	struct GlobalUbo
	{
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize( glm::vec3{1.0f, -3.0f, -1.0f} );
	};
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
		std::array<std::unique_ptr<weEngineBuffer>, weEngineSwapChain::MAX_FRAMES_IN_FLIGHT> uboBuffers{};
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<weEngineBuffer>(
				weEngineDevice,
				sizeof(GlobalUbo),
				1, //Synchronize assigning the UBO with each of the frame
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				//weEngineDevice.properties.limits.minUniformBufferOffsetAlignment
			);
			uboBuffers[i]->map();
		}

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
				int frameIndex = weEngineRenderer.getCurrentFrameIndex();

				FrameInfo frameInfo
				{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				//Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//Render
				weEngineRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, gameObjects);
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
		gameObjects.reserve(2);
		std::shared_ptr<weEngineModel> weEngineModel = weEngineModel::createModelFromFile(weEngineDevice, "models\\colored_cube.obj");

		auto gameObj = weEngineGameObject::createGameObject();

		gameObj.model = weEngineModel;
		gameObj.transformComp.translation = { 0.0f, 0.0f, 5.0f };
		gameObj.transformComp.scale = { 0.5f, 0.5f, 0.5f };

		gameObjects.push_back(std::move(gameObj));

		std::shared_ptr<weEngine::weEngineModel> model = weEngineModel::createModelFromFile(weEngineDevice, "models\\flat_vase.obj");

		auto gameObj2 = weEngineGameObject::createGameObject();

		gameObj2.model = model;
		gameObj2.transformComp.translation = { 0.0f, 0.0f, 2.5f };
		gameObj2.transformComp.scale = { 1.2f, 1.2f, 1.2f };
		
		gameObjects.push_back(std::move(gameObj2));
	}




}