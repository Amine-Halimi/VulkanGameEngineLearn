#pragma once

#include "weEngineCamera.hpp"

#include "vulkan/vulkan.h"

namespace weEngine
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		weEngineCamera& camera;
	};
}