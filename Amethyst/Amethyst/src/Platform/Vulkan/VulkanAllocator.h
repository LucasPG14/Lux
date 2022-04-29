#pragma once

#include "include/vk_mem_alloc.h"

namespace Amethyst
{
	class VulkanAllocator
	{
	public:
		VulkanAllocator();

		~VulkanAllocator();

		void AllocateBuffer(VkBufferCreateInfo bufferInfo, VmaMemoryUsage usage);
	private:
		VmaAllocator alloc;
	};
}