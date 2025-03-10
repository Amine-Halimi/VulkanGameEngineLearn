#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "string"

/*
* 
* weEngineWindow is a wrapper class for a private GLFWwindow pointer. This wraps around the functions from glfw to use them.
* 
* author: Amine Halimi
*/


//Set up a namespace weEngine for setting up our classes
namespace weEngine {
	class weEngineWindow
	{

	public:
		weEngineWindow(int width, int height, std::string name);
		~weEngineWindow();

		weEngineWindow(const weEngineWindow&) = delete;
		weEngineWindow& operator=(const weEngineWindow&) = delete;
		bool shouldClose() {
			return glfwWindowShouldClose(window);
		};
		VkExtent2D getExtent() { return { static_cast<uint32_t>(widthWindow), static_cast<uint32_t>(heightWindow) }; };
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		bool wasWindowResized() { return framebufferResized; };
		void resetWindowResizedFlags() { framebufferResized = false; };


	private:
		static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int heightWindow;
		int widthWindow;
		bool framebufferResized = false;

		std::string windowTitle;
		GLFWwindow* window;
	};
}