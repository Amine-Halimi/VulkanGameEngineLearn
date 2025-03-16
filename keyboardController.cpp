#include "keyboardController.hpp"

//std
#include "limits"
#include "iostream"
//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"


namespace weEngine
{
	void KeyboardMovementController::moveInPlaceXZ(GLFWwindow* window, float dt, weEngineGameObject& gameObj)
	{
		glm::vec3 rotation{ 0.0f };

		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
		{
			rotation.y += 1.0f;
		}
			
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
		{
			rotation.y -= 1.0f;
		}
			
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
		{
			rotation.x += 1.0f;
		}
			
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
		{
			rotation.x -= 1.0f;
		}
			

		//Ensuring that the rotation vector is nonzero
		if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
			gameObj.transformComp.rotation += turnSpeed * dt * glm::normalize(rotation);

		gameObj.transformComp.rotation.x = glm::clamp(gameObj.transformComp.rotation.x, -1.5f, 1.5f);
		gameObj.transformComp.rotation.y = glm::mod(gameObj.transformComp.rotation.y, glm::two_pi<float>());

		float yaw = gameObj.transformComp.rotation.y;
		const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x};
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };

		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
			moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
			moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
			moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
			moveDir += rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
			moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
			moveDir -= upDir;

		//Ensuring that the moveDir vector is nonzero
		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
			gameObj.transformComp.translation += turnSpeed * dt * glm::normalize(moveDir);
	}
}