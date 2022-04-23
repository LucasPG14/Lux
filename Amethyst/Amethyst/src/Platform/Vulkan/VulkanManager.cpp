#include "amtpch.h"
#include "VulkanManager.h"

namespace Amethyst
{
	VkPhysicalDevice VulkanManager::physicalDevice = VK_NULL_HANDLE;
	VkDevice VulkanManager::device = VK_NULL_HANDLE;
	VkAllocationCallbacks* VulkanManager::callbackAllocator = VK_NULL_HANDLE;
}