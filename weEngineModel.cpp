#include "weEngineModel.hpp"
#include "weEngineUtils.hpp"

//std
#include "cassert"
#include "cstring"
#include "unordered_map"

//GLM
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

//TinyObjLoader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

namespace std
{
	template<>
	struct hash<weEngine::weEngineModel::Vertex> 
	{
		size_t operator()(weEngine::weEngineModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			weEngine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);

			return seed;
		}
	};
}


namespace weEngine
{
	weEngineModel::weEngineModel(weEngine::weEngineDevice& device, const weEngineModel::Builder& modelBuilder) :weEngineDevice(device)
	{
		createVertexBuffers(modelBuilder.vertices);
		createIndexBuffers(modelBuilder.indices);
	}

	weEngineModel::~weEngineModel()
	{
		vkDestroyBuffer(weEngineDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(weEngineDevice.device(), vertexBufferMemory, nullptr);

		if (hasIndices)
		{
			vkDestroyBuffer(weEngineDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(weEngineDevice.device(), indexBufferMemory, nullptr);
		}
	}
	/*
	* Create vertex buffers and allocate memory for it. Create staging buffer to temporarily stored the data before transferring it to the GPU
	*/
	void weEngineModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		weEngineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		/*
		* Sends the data to the GPU
		*/
		void* data;
		vkMapMemory(weEngineDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(weEngineDevice.device(), stagingBufferMemory);

		weEngineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		weEngineDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(weEngineDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(weEngineDevice.device(), stagingBufferMemory, nullptr);
	}

	/*
	* Creates an index buffer inside the GPU
	*/
	void weEngineModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndices = indexCount > 0;

		if (!hasIndices)
		{
			return;
		}
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		weEngineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		/*
		* Sends the data to the GPU
		*/
		void* data;
		vkMapMemory(weEngineDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(weEngineDevice.device(), stagingBufferMemory);

		weEngineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory
		);

		weEngineDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(weEngineDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(weEngineDevice.device(), stagingBufferMemory, nullptr);
	}

	/*
	* Enters the draw command into the commandbuffer
	*/
	void weEngineModel::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndices)
		{
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
		
	}

	/*
	* Binds the command buffer to the model
	*/
	void weEngineModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndices)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	/*
	* Returns the pointer of a weEngineModel object from a path to a 3D model (.obj file).
	*/
	std::unique_ptr<weEngineModel> weEngineModel::createModelFromFile(weEngine::weEngineDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);

		return std::make_unique<weEngineModel>(device, builder);

	}
	/*
	* Describes how the input binding inside the buffer data is formatted
	*/
	std::vector<VkVertexInputBindingDescription> weEngineModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescription(1);
		bindingDescription[0].binding = 0;
		bindingDescription[0].stride = sizeof(Vertex);
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	/*
	* Describes how the attributes stored in the input binding 
	*/

	std::vector<VkVertexInputAttributeDescription> weEngineModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescription(2);
		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex, position);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex, color);

		return attributeDescription;
	}

	/*
	* Loads the model use tinyobj::loadObj and storing it temporarily inside attrib, shapes and materials
	*/
	void weEngineModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err, warn;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> unique_vertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position =
					{
						attrib.vertices[3 * index.vertex_index + 0], //x component
						attrib.vertices[3 * index.vertex_index + 1], //y component
						attrib.vertices[3 * index.vertex_index + 2], //z component
					};
					auto colorIndex = 3 * index.vertex_index + 2;
					if (colorIndex < attrib.colors.size())
					{
						vertex.color =
						{
							attrib.colors[colorIndex - 2], //x component
							attrib.colors[colorIndex - 1], //y component
							attrib.colors[colorIndex - 0], //z component
						};
					}
					else
					{
						vertex.color = { 1.0f, 1.0f, 1.0f }; //default color
					}
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0], //x component
						attrib.normals[3 * index.normal_index + 1], //y component
						attrib.normals[3 * index.normal_index + 2], //z component
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv =
					{
						attrib.texcoords[2 * index.texcoord_index + 0], //x component
						attrib.texcoords[2 * index.texcoord_index + 1], //y component
					};
				}

				if (unique_vertices.count(vertex) == 0)
				{
					unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(unique_vertices[vertex]);
			}
		}
	}
	
}