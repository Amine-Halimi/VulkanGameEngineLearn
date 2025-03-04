#include "weEngineModel.hpp"

//std
#include "cassert"
#include "cstring"
namespace weEngine
{
	weEngineModel::weEngineModel(weEngine::weEngineDevice& device, const std::vector<Vertex>& vertices) :weEngineDevice(device)
	{
		createVertexBuffers(vertices);
	}

	weEngineModel::~weEngineModel()
	{
		vkDestroyBuffer(weEngineDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(weEngineDevice.device(), vertexBufferMemory, nullptr);
	}
	/*
	* Create vertex buffers and allocate memory for it
	*/
	void weEngineModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		weEngineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		/*
		* Sends the data to the GPU
		*/
		void* data;
		vkMapMemory(weEngineDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(weEngineDevice.device(), vertexBufferMemory);
	}
	/*
	* Enters the draw command into the commandbuffer
	*/
	void weEngineModel::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	/*
	* Binds the command buffer to the model
	*/
	void weEngineModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
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
		attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex, position);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex, color);

		return attributeDescription;
	}
}