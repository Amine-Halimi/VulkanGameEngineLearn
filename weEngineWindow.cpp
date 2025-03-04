#include "weEngineWindow.hpp"
#include "stdexcept"

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

	void weEngineWindow::initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		window = glfwCreateWindow(widthWindow, heightWindow, windowTitle.c_str(), nullptr, nullptr);
	}

	void weEngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
}