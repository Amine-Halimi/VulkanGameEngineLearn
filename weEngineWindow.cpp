#include "weEngineWindow.hpp"
#include "stdexcept"

/*
* weEngineWindow implementation. Contains a glfw window for the engine to use
* 
* author: Amine Halimi
*/

namespace weEngine {
	weEngineWindow::weEngineWindow(int width, int height, std::string name): widthWindow{width}, heightWindow{height}, windowTitle{name}
	{
		initWindow();
	}

	weEngineWindow::~weEngineWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	/*
	* Initialize a glfw window pointer and set up a framebuffer resized callback function.
	*/
	void weEngineWindow::initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		window = glfwCreateWindow(widthWindow, heightWindow, windowTitle.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);

	}

	/*
	* Create a window surface for a given vulkan instance
	*/
	void weEngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	/*
	* Callback function for resizing windows
	*/
	void weEngineWindow::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto weEngineWindow = reinterpret_cast<weEngine::weEngineWindow*>(glfwGetWindowUserPointer(window));
		weEngineWindow->framebufferResized = true;
		weEngineWindow->widthWindow = width;
		weEngineWindow->heightWindow = height;
	}
}