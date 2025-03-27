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
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
