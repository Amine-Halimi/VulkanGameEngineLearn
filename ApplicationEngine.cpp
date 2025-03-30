#include "ApplicationEngine.hpp"
#include "SimpleRenderingSystem.hpp"
#include "keyboardController.hpp"
#include "mouseController.hpp"
#include "weEngineBuffer.hpp"
#include "weEngineFrameInfo.hpp"
#include "pointlightSystem.hpp"

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
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		//glm::vec3 lightDirection = glm::normalize( glm::vec3{1.0f, -3.0f, -1.0f} );
		glm::vec4 ambientColorLight{ 1.f, 1.f, 1.f, .02f };
		glm::vec3 lightPosition{-1.0f};
		alignas(16) glm::vec4 lightColor{ 1.0f }; //w is the light intensity
	};
	std::unique_ptr<weEngineModel> createCubeModel(weEngineDevice& device, glm::vec3 offset);

	ApplicationEngine::ApplicationEngine()
	{
		globalPool = weEngineDescriptorPool::Builder(weEngineDevice)
			.setMaxSets(weEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, weEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	ApplicationEngine::~ApplicationEngine()
	{
	}

	/*
	* Main loop of the application engine
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

		//Create a descriptor set layout
		auto globalSetLayout = weEngineDescriptorSetLayout::Builder(weEngineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(weEngineSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			weEngineDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderingSystem renderSystem{ weEngineDevice, weEngineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{ weEngineDevice, weEngineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		weEngineCamera camera{};
		
		auto currentTime = std::chrono::high_resolution_clock::now();

		auto cameraObject = weEngineGameObject::createGameObject();
		cameraObject.transformComp.translation.z = -2.5f;
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
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//Update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//Render
				weEngineRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
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
		gameObjects.reserve(3);
		std::shared_ptr<weEngineModel> coloredCubeModel = weEngineModel::createModelFromFile(weEngineDevice, "models\\colored_cube.obj");

		auto coloredCube = weEngineGameObject::createGameObject();

		coloredCube.model = coloredCubeModel;
		coloredCube.transformComp.translation = { 0.0f, -0.05f, 1.5f };
		coloredCube.transformComp.scale = { 0.5f, 0.5f, 0.5f };

		gameObjects.emplace(coloredCube.getId(), std::move(coloredCube));

		std::shared_ptr<weEngine::weEngineModel> vaseModel = weEngineModel::createModelFromFile(weEngineDevice, "models\\flat_vase.obj");

		auto flatVase = weEngineGameObject::createGameObject();

		flatVase.model = vaseModel;
		flatVase.transformComp.translation = { 0.0f, 0.0f, 0.5f };
		flatVase.transformComp.scale = { 1.2f, 1.2f, 1.2f };
		
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		std::shared_ptr<weEngine::weEngineModel> quad = weEngineModel::createModelFromFile(weEngineDevice, "models\\floor.obj");

		auto gameObjFloor = weEngineGameObject::createGameObject();

		gameObjFloor.model = quad;
		gameObjFloor.transformComp.translation = { 0.0f, 0.5f, 0.0f };
		gameObjFloor.transformComp.scale = { 3.f, 1.f, 3.f };

		gameObjects.emplace(gameObjFloor.getId(), std::move(gameObjFloor));
	}




}