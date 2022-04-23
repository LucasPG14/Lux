#pragma once

#include <vulkan/vulkan.h>

namespace Amethyst
{
	class VulkanManager
	{
	public:
		// Setters
		static void SetPhysicalDevice(const VkPhysicalDevice& physicalDev) { physicalDevice = physicalDev; }
		static void SetDevice(const VkDevice& dev) { device = dev; }
		static void SetCallbackAllocator(VkAllocationCallbacks* callbackAlloc) { callbackAllocator = callbackAlloc; }

		// Getters
		static const VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
		static const VkDevice& GetDevice() { return device; }
		static const VkAllocationCallbacks* GetCallbackAllocator() { return callbackAllocator; }
	private:
		static VkPhysicalDevice physicalDevice;
		static VkDevice device;
		static VkAllocationCallbacks* callbackAllocator;
	};
}