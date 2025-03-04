#pragma once

/*
* Author: Amine Halimi
* Purpose of this code is to read vertices data and allocate memory space and copy data over to render it.
*/

#include "weEngineDevice.hpp"

//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

//std
#include "vector"

namespace weEngine
{
	class weEngineModel
	{
	public:

		struct Vertex {
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		weEngineModel(weEngineDevice& device, const std::vector<Vertex>& vertices);
		~weEngineModel();

		weEngineModel(const weEngineModel&) = delete;
		weEngineModel& operator=(const weEngineModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);

		weEngineDevice& weEngineDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}