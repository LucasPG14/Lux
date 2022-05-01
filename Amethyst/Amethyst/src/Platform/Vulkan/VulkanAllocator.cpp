#include "amtpch.h"
#include "VulkanAllocator.h"

#include "VulkanContext.h"

namespace Amethyst
{
	VmaAllocator VulkanAllocator::allocator = VK_NULL_HANDLE;

	VulkanAllocator::VulkanAllocator(const std::string& n) : name(n)
	{
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_0;
		allocatorCreateInfo.physicalDevice = VulkanContext::GetVkDevice().GetPhysicalDevice();
		allocatorCreateInfo.device = VulkanContext::GetVkDevice().GetDevice();
		allocatorCreateInfo.instance = VulkanContext::GetInstance();

		vmaCreateAllocator(&allocatorCreateInfo, &allocator);
	}

	VulkanAllocator::~VulkanAllocator()
	{
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferInfo, VkMemoryPropertyFlagBits usage, VkBuffer& buffer)
	{
		VmaAllocation allocation;

		VmaAllocationCreateInfo allocCreateInfo = {};
		//allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocCreateInfo.requiredFlags = usage;
		allocCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
		vmaSetAllocationName(allocator, allocation, name.c_str());

		return allocation;
	}
	
	void* VulkanAllocator::MapMemory(VmaAllocation allocation)
	{
		void* mappedData;
		vmaMapMemory(allocator, allocation, &mappedData);

		return mappedData;
	}
	
	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(allocator, allocation);
	}
}