#include "amtpch.h"
#include "VulkanVertexBuffer.h"

#include "VulkanContext.h"
#include "VulkanAllocator.h"

namespace Amethyst
{
	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size) : vertexBuffer(VK_NULL_HANDLE)
	{
		// TODO: Use staging buffers
		VkBufferCreateInfo vertexBufferInfo = {};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.size = size;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VulkanAllocator allocator("VertexBuffer");

		auto bufferAllocation = allocator.AllocateBuffer(vertexBufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer);

		void* dstBuffer = allocator.MapMemory(bufferAllocation);
		memcpy(dstBuffer, data, size);
		allocator.UnmapMemory(bufferAllocation);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}

}