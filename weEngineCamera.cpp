

#include "weEngineCamera.hpp"


//std
#include "cassert"
#include "limits"

namespace weEngine
{
	/*
	* Sets the projectionMatrix to an orthogonal projection matrix. Gets the float values left, right, top, bottom, near and far sides of the projection box
	*/
	void weEngineCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
	{
		projectionMatrix = glm::mat4{ 1.0f };
		projectionMatrix[0][0] = 2.0f / (right - left);
		projectionMatrix[1][1] = 2.0f / (bottom - top);
		projectionMatrix[2][2] = 1.0f / (far - near);
		projectionMatrix[3][0] = -1.0f * (right + left) / (right - left);
		projectionMatrix[3][1] = -1.0f * (bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -1.0f * near / (far - near);
	}

	/*
	* Sets the projectionMatrix to a perspective projection matrix. Gets the float values for the field of view, the aspect ratio of the viewport 
	* and the near and far sides of the projection box.
	*/
	void weEngineCamera::setPerspectiveProjection(float fov_on_y, float aspectRatio, float near, float far)
	{
		assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f && "");
		const float tanHalfFov = glm::tan(fov_on_y / 2.0f);

		projectionMatrix = glm::mat4{ 0.0f };
		projectionMatrix[0][0] = 1.0f / (aspectRatio * tanHalfFov);
		projectionMatrix[1][1] = 1.0f / tanHalfFov;
		projectionMatrix[2][2] = far / (far - near);
		projectionMatrix[3][2] = -1.0f * far * near / (far - near);
		projectionMatrix[2][3] = 1.0f;
	}

	/*
	* Sets the viewMatrix at a given position in space, pointing to a specific direction and the up vector of the world.
	* Creates an orthonormal basis for the view matrix. 
	*/
	void weEngineCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
	{
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);
	}

	void weEngineCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	{
		assert(target != position && "target position vector and camera position vector are the same.");
		setViewDirection(position, target - position, up);
	}
	/*
	* Function rotates the camera view with the given rotation vector. 
	*/
	void weEngineCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };

		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);
	}
}