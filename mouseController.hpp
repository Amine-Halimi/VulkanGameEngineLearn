#pragma once

#include "weEngineGameObject.hpp"
#include "weEngineWindow.hpp"

namespace weEngine
{
	class MouseMovementController
	{
	public:
		void processMouseMovement(GLFWwindow* window, weEngineGameObject& gameObj);

		float mouseSensitivity{ 0.05f };

	private:
		const float MAX_PITCH{ 1.5f };
		const float MIN_PITCH{ -1.5f };
		bool firstTimeMovingMouse{ true };

		float lastX{ 0.0f };
		float lastY{ 0.0f };
	};
}