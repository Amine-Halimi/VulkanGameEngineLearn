#pragma once

#include "weEngineDevice.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include "memory"
#include <string>
#include <vector>

/*
* Vulkan swapchain object created and modified by the tutorial
* 
* author: Brendan Galea
* 
*/


namespace weEngine {

class weEngineSwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  weEngineSwapChain(weEngineDevice &deviceRef, VkExtent2D windowExtent);
  weEngineSwapChain(weEngineDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<weEngineSwapChain> previous);
  ~weEngineSwapChain();

  weEngineSwapChain(const weEngineSwapChain &) = delete;
  weEngineSwapChain &operator=(const weEngineSwapChain &) = delete;

  VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
  VkRenderPass getRenderPass() { return renderPass; }
  VkImageView getImageView(int index) { return swapChainImageViews[index]; }
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  /*
  * Returns if two swap chains are compatible
  */
  bool compareSwapFormats(const weEngineSwapChain& swapChain) const
  {
      return swapChain.swapChainImageFormat == swapChainImageFormat && swapChain.swapChainDepthFormat == swapChainDepthFormat;
  }

 private:
  void init();
  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  weEngineDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  std::shared_ptr<weEngineSwapChain> oldSwapChain;


  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;
};

}  // namespace weEngine
