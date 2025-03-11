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
//#include "glm/gtc/matrix_transform.hpp"

//std
#include "memory"
namespace weEngine
{
	struct Transform2DComponent
	{
		glm::vec2 translation;
		glm::vec2 scale{ 1.1f, 1.1f };
		float rotation = 0;

		glm::mat2 mat2()
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotateMatrix{ {c, s}, {-s, c} };
			
			glm::mat2 scaleMatrix{ { scale.x, 0.0f}, {0.0f, scale.y} };


			return rotateMatrix * scaleMatrix;
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
		Transform2DComponent transform2d{};

	private:
		weEngineGameObject(id_t objId) : id{ objId } {};
		id_t id;

	};

}
