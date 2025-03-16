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
#include "memory"


namespace weEngine
{
	class weEngineModel
	{
	public:
		//Struct for storing the vertex data
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		//Holds the vertex data and the indices for each triangles
		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string &filepath);
		};

		weEngineModel(weEngineDevice& device, const weEngineModel::Builder& modelBuilder);
		~weEngineModel();

		weEngineModel(const weEngineModel&) = delete;
		weEngineModel& operator=(const weEngineModel&) = delete;

		static std::unique_ptr<weEngineModel> createModelFromFile(weEngineDevice& device, const std::string &filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		weEngineDevice& weEngineDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndices = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}