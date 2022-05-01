#pragma once

#include "vulkan/vulkan.h"

namespace Amethyst
{
	class VulkanIndexBuffer
	{
	public:
		VulkanIndexBuffer(void* data, uint32_t size);

		VkBuffer GetIndexBuffer() { return indexBuffer; }
		uint32_t GetCount() { return count; }
	private:
		VkBuffer indexBuffer;
		uint32_t count;
	};
}