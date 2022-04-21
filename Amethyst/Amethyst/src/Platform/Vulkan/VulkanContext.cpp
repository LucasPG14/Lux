#include "amtpch.h"
#include "VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Amethyst
{
	static void CheckVkResult(VkResult error)
	{
		AMT_CORE_ASSERT(!error, "Vulkan error");
	}

	VulkanContext::VulkanContext(GLFWwindow* currentWindow) 
		: window(currentWindow), context(VK_NULL_HANDLE), callbacksAllocator(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), queueCount(-1),
		queue(VK_NULL_HANDLE)
	{
	}
	
	void VulkanContext::Init()
	{
		VkResult error;

		// Getting the extensions of Vulkan
		{
			uint32_t extensionsCount = 0;
			const char** extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

			// Setting the information of the context
			VkInstanceCreateInfo instanceCreateInfo = {};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.enabledExtensionCount = extensionsCount;
			instanceCreateInfo.ppEnabledExtensionNames = extensions;

			// Creating the context of Vulkan and checking for errors
			error = vkCreateInstance(&instanceCreateInfo, callbacksAllocator, &context);
			CheckVkResult(error);
		}

		// ---------------- Selecting the GPU ------------------
		{
			// Checking the number of GPUs
			uint32_t gpuCount = 0;
			error = vkEnumeratePhysicalDevices(context, &gpuCount, NULL);
			CheckVkResult(error);

			AMT_CORE_ASSERT(gpuCount > 0, "Couldn't find a single GPU");

			// Getting the list of all the gpus on the system
			VkPhysicalDevice* gpus = new VkPhysicalDevice[gpuCount];
			error = vkEnumeratePhysicalDevices(context, &gpuCount, gpus);
			CheckVkResult(error);

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
			delete[] gpus;
		}

		// ------------- Selecting the GPU queue ---------------
		{
			uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
			VkQueueFamilyProperties* queuesProperties = new VkQueueFamilyProperties[count];
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queuesProperties);

			// Iterating through all the queues
			for (uint32_t i = 0; i < count; ++i)
			{
				if (queuesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueCount = i;
					break;
				}
			}

			delete[] queuesProperties;
			AMT_CORE_ASSERT(queueCount != -1, "Queues Family not found");
		}
		
		// ----------- Creating the device with the queue -----------
		{
			int devExtensionCount = 1;
			const char* extensions[] = { "VK_KHR_swapchain" };
			float queuePriority[] = {1.0f};

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
			error = vkCreateDevice(physicalDevice, &deviceInfo, callbacksAllocator, &device);
			CheckVkResult(error);
			vkGetDeviceQueue(device, queueCount, 0, &queue);
		}

		// -------------- Setting the descriptor pools --------------
		{

		}
	}

	void VulkanContext::Shutdown()
	{
		vkDestroyDevice(device, callbacksAllocator);
		vkDestroyInstance(context, callbacksAllocator);
	}
	
	void VulkanContext::SwapBuffers()
	{

	}
}