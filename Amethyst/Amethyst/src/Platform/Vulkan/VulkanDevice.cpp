#include "amtpch.h"
#include "VulkanDevice.h"

#include "VulkanContext.h"

namespace Amethyst
{
	VulkanDevice::VulkanDevice() : device(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), queueCount(0)
	{
	}

	VulkanDevice::~VulkanDevice()
	{
	}
	
	void VulkanDevice::Init()
	{
		CreatePhysicalDevice();

		// ------------- Selecting the GPU queue ---------------
		{
			uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
			std::vector<VkQueueFamilyProperties> queuesProperties;
			queuesProperties.resize(count);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queuesProperties.data());

			// Iterating through all the queues
			for (uint32_t i = 0; i < count; ++i)
			{
				if (queuesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueCount = i;
					break;
				}
			}

			AMT_CORE_ASSERT(queueCount != -1, "Queues Family not found");
		}

		CreateDevice();
	}

	void VulkanDevice::Shutdown()
	{
		vkDestroyDevice(device, nullptr);
	}
	
	void VulkanDevice::CreateDevice()
	{
		// ----------- Creating the device with the queue -----------
		{
			int devExtensionCount = 1;
			const char* extensions[] = { "VK_KHR_swapchain" };
			float queuePriority[] = { 1.0f };

			// Setting device queue information
			VkDeviceQueueCreateInfo queueInfo[1] = {};
			queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo[0].queueFamilyIndex = queueCount;
			queueInfo[0].queueCount = 1;
			queueInfo[0].pQueuePriorities = queuePriority;

			// Setting device information
			VkDeviceCreateInfo deviceInfo = {};
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceInfo.queueCreateInfoCount = sizeof(queueInfo) / sizeof(queueInfo[0]);
			deviceInfo.pQueueCreateInfos = queueInfo;
			deviceInfo.enabledExtensionCount = devExtensionCount;
			deviceInfo.ppEnabledExtensionNames = extensions;

			// Finally creating the device
			VkResult error = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
		}
	}

	void VulkanDevice::CreatePhysicalDevice()
	{
		// ---------------- Selecting the GPU ------------------
		{
			// Checking the number of GPUs
			uint32_t gpuCount = 0;
			VkResult error = vkEnumeratePhysicalDevices(VulkanContext::GetInstance(), &gpuCount, NULL);

			AMT_CORE_ASSERT(gpuCount > 0, "Couldn't find a single GPU");

			// Getting the list of all the gpus on the system
			std::vector<VkPhysicalDevice> gpus;
			gpus.resize(gpuCount);
			error = vkEnumeratePhysicalDevices(VulkanContext::GetInstance(), &gpuCount, gpus.data());

			// Searching for a dedicated GPU
			// If not, the GPU integrated on the CPU will be selected
			uint32_t index = 0;
			for (uint32_t i = 0; i < gpuCount; ++i)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(gpus[i], &properties);
				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					index = i;
					break;
				}
			}

			physicalDevice = gpus[index];
		}
	}
}