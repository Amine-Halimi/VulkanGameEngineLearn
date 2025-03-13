#pragma once
/*
* 
* author: Amine Halimi
*/

#include "weEngineWindow.hpp"
#include "weEngineDevice.hpp"
#include "weEngineSwapChain.hpp"

//std
#include "memory"
#include "vector"
#include "cassert"

namespace weEngine
{
	class weEngineRenderer
	{
	public:

		weEngineRenderer(weEngineWindow& window, weEngineDevice& device);
		~weEngineRenderer();

		weEngineRenderer(const weEngineRenderer&) = delete;
		weEngineRenderer& operator=(const weEngineRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const
		{
			return weEngineSwapChain->getRenderPass();
		}

		float getAspectRatio() const
		{
			return weEngineSwapChain->extentAspectRatio();
		}
		bool isFrameInProgress() const
		{
			return isFrameStarted;
		}

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot retrieve current command buffer since the frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getCurrentFrameIndex() const
		{
			assert(isFrameStarted && "Cannot retrieve currentFrameIndex while the frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		weEngineWindow& weEngineWindow;
		weEngineDevice& weEngineDevice;
		std::unique_ptr<weEngineSwapChain> weEngineSwapChain; // weEngineDevice, weEngineWindow.getExtent()
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}