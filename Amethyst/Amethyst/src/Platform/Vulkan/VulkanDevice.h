#pragma once

#include <vulkan/vulkan.h>

namespace Amethyst
{
	class VulkanDevice
	{
	public:
		VulkanDevice();
		~VulkanDevice();

		void Init();
		void Shutdown();

		VkDevice& GetDevice() { return device; }
		VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
		
		uint32_t GetQueueCount() { return queueCount; }

	private:
		void CreateDevice();
		void CreatePhysicalDevice();

	private:
		VkDevice device;
		VkPhysicalDevice physicalDevice;

		uint32_t queueCount;
	};
}