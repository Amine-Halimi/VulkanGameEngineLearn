#include "mouseController.hpp"

//std
#include "limits"
#include "iostream"
//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

namespace weEngine
{

	void MouseMovementController::processMouseMovement(GLFWwindow* window, weEngineGameObject& gameObj)
	{
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double mouseX;
			double mouseY;

			glfwGetCursorPos(window, &mouseX, &mouseY);

			if (firstTimeMovingMouse)
			{
				lastX = static_cast<float>(mouseX);
				lastY = static_cast<float>(mouseY);
				firstTimeMovingMouse = false;
			}

			glm::vec3 rotation{ 0.0f };
			rotation.y = static_cast<float>(mouseX) - lastX;
			rotation.x = static_cast<float>(mouseY) - lastY;

			lastX = static_cast<float>(mouseX);
			lastY = static_cast<float>(mouseY);

			rotation.y *= 1.0f;
			rotation.x *= -1.0f;

			//Ensuring that the rotation vector is nonzero
			if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
				gameObj.transformComp.rotation += mouseSensitivity * glm::normalize(rotation);

			gameObj.transformComp.rotation.x = glm::clamp(gameObj.transformComp.rotation.x, -1.5f, 1.5f);
			gameObj.transformComp.rotation.y = glm::mod(gameObj.transformComp.rotation.y, glm::two_pi<float>());

		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstTimeMovingMouse = true;
		}

	}
}