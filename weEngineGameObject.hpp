#pragma once

/*
* 
* Game object class implements an object in the game. It contains a reference to weEngineModel
* 
* author: Amine Halimi
*/

#include "weEngineModel.hpp"

//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

//std
#include "memory"
namespace weEngine
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		/*
		* Represents the transform matrix for a gameobject Uses the Tait-Bryan angle Y1X2Z3
		* https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		*/
		glm::mat4 mat4()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f} };
		}
	};

	class weEngineGameObject 
	{
	public:
		using id_t = unsigned int;

		//Static function creating a game object
		static weEngineGameObject createGameObject()
		{
			static id_t currentId = 0;
			return weEngineGameObject(currentId++);
		}

		id_t getId() const
		{
			return id;
		}

		weEngineGameObject(const weEngineGameObject&) = delete;
		weEngineGameObject& operator=(const weEngineGameObject&) = delete;

		weEngineGameObject(weEngineGameObject&&) = default;
		weEngineGameObject& operator=(weEngineGameObject&&) = default;

		std::shared_ptr<weEngineModel> model{};
		glm::vec3 color{};
		TransformComponent transformComp{};

	private:
		weEngineGameObject(id_t objId) : id{ objId } {};
		id_t id;

	};

}
