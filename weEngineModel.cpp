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
	}
	/*
	* Create vertex buffers and allocate memory for it. Create staging buffer to temporarily stored the data before transferring it to the GPU
	*/
	void weEngineModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		weEngineBuffer stagingBuffer{
			weEngineDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		/*
		* Sends the data to the GPU
		*/
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());
		
		vertexBuffer = std::make_unique<weEngineBuffer>(
			weEngineDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		weEngineDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
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
		uint32_t indexSize = sizeof(indices[0]);

		weEngineBuffer stagingBuffer{
			weEngineDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		/*
		* Sends the data to the GPU
		*/
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<weEngineBuffer>(
			weEngineDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);


		weEngineDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
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
		VkBuffer buffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndices)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
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
		std::vector<VkVertexInputAttributeDescription> attributeDescription{};
		attributeDescription.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescription.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescription.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescription.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

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
					
					vertex.color =
					{
						attrib.colors[3 * index.vertex_index + 0], //x component
						attrib.colors[3 * index.vertex_index + 1], //y component
						attrib.colors[3 * index.vertex_index + 2], //z component
					};
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