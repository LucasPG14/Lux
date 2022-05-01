#pragma once

#include "include/vk_mem_alloc.h"

namespace Amethyst
{
	class VulkanAllocator
	{
	public:
		VulkanAllocator(const std::string& n);

		~VulkanAllocator();

		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferInfo, VkMemoryPropertyFlagBits usage, VkBuffer& buffer);

		void* MapMemory(VmaAllocation allocation);
		void UnmapMemory(VmaAllocation allocation);
	private:
		static VmaAllocator allocator;

		std::string name;
	};
}