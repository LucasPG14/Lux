#include "amtpch.h"
#include "VulkanIndexBuffer.h"

#include "VulkanAllocator.h"

namespace Amethyst
{
	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t size) : indexBuffer(VK_NULL_HANDLE), count(size / sizeof(uint32_t))
	{
		VkBufferCreateInfo vertexBufferInfo = {};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.size = size;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VulkanAllocator allocator("IndexBuffer");

		auto bufferAllocation = allocator.AllocateBuffer(vertexBufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer);

		void* dstBuffer = allocator.MapMemory(bufferAllocation);
		memcpy(dstBuffer, data, size);
		allocator.UnmapMemory(bufferAllocation);
	}

}