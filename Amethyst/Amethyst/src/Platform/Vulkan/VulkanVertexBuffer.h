#pragma once

#include "vulkan/vulkan.h"

namespace Amethyst
{
	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size);

		~VulkanVertexBuffer();

		VkBuffer* GetVertexBuffer() { return &vertexBuffer; }

	private:
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
	};
}