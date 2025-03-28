#pragma once

//weEngine
#include "weEngineCamera.hpp"
#include "weEngineGameObject.hpp"

//Vulkan
#include "vulkan/vulkan.h"

namespace weEngine
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		weEngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		weEngineGameObject::Map& gameObjects;
	};
}