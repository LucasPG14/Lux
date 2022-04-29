#pragma once

#include "vulkan/vulkan.h"

namespace Amethyst
{
	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer();

		~VulkanVertexBuffer();

	private:
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
	};
}