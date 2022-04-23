#include "amtpch.h"
#include "VulkanContext.h"

#include "VulkanManager.h"

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
		VulkanManager::SetCallbackAllocator(callbacksAllocator);
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

			VulkanManager::SetPhysicalDevice(physicalDevice);
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

			VulkanManager::SetDevice(device);
		}

		// -------------- Setting the descriptor pools --------------
		{

		}

		// ------------------ Creating the surface ------------------
		{
			error = glfwCreateWindowSurface(context, window, callbacksAllocator, &surface);
			CheckVkResult(error);
		}

		// ----------------- Creating the swapchain -----------------
		{
			// Getting surface capabilities
			VkSurfaceCapabilitiesKHR capabilities;
			error = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
			CheckVkResult(error);

			extent = capabilities.currentExtent;

			// Getting surface formats
			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);

			AMT_CORE_ASSERT(formatCount != 0, "There aren't any surface formats");

			VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats);

			// Getting surface present modes
			uint32_t presentCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, NULL);

			AMT_CORE_ASSERT(presentCount != 0, "There aren't any surface present modes");

			VkPresentModeKHR* presentModes = new VkPresentModeKHR[presentCount];
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, presentModes);

			// Selecting the surface format
			int formatIndex = 0;
			for (uint32_t i = 0; i < formatCount; ++i)
			{
				if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					formatIndex = i;
					break;
				}
			}

			format = surfaceFormats[formatIndex].format;

			// Selecting the surface present mode
			int presentIndex = 0;
			for (uint32_t i = 0; i < presentCount; ++i)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentIndex = i;
					break;
				}
			}

			// Setting the image count
			uint32_t imageCount = capabilities.minImageCount + 1;

			// Creating the swapchain info
			VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
			swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapChainCreateInfo.surface = surface;
			swapChainCreateInfo.minImageCount = imageCount;
			swapChainCreateInfo.imageFormat = format;
			swapChainCreateInfo.imageColorSpace = surfaceFormats[formatIndex].colorSpace;
			swapChainCreateInfo.imageExtent = extent;
			swapChainCreateInfo.imageArrayLayers = 1;
			swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapChainCreateInfo.preTransform = capabilities.currentTransform;
			swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapChainCreateInfo.presentMode = presentModes[presentIndex];
			swapChainCreateInfo.clipped = VK_TRUE;
			swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

			// Finally creating the swapchain
			error = vkCreateSwapchainKHR(device, &swapChainCreateInfo, callbacksAllocator, &swapChain);
			CheckVkResult(error);

			// Getting the swapchain images
			vkGetSwapchainImagesKHR(device, swapChain, &imgCount, NULL);
			images = new VkImage[imgCount];
			error = vkGetSwapchainImagesKHR(device, swapChain, &imgCount, images);
			CheckVkResult(error);

			// Creating the image view for the swapchain
			imageViews = new VkImageView[imgCount];
			for (uint32_t i = 0; i < imgCount; ++i)
			{
				VkImageViewCreateInfo imgViewCreateInfo = {};
				imgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imgViewCreateInfo.image = images[i];
				imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imgViewCreateInfo.format = format;

				imgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				
				imgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imgViewCreateInfo.subresourceRange.levelCount = 1;
				imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imgViewCreateInfo.subresourceRange.layerCount = 1;

				error = vkCreateImageView(device, &imgViewCreateInfo, callbacksAllocator, &imageViews[i]);
				CheckVkResult(error);
			}
		}


	}

	void VulkanContext::Shutdown()
	{
		for (uint32_t i = 0; i < imgCount; ++i)
		{
			vkDestroyImageView(device, imageViews[i], callbacksAllocator);
		}

		vkDestroySwapchainKHR(device, swapChain, callbacksAllocator);
		vkDestroyDescriptorPool(device, descriptorPool, callbacksAllocator);
		vkDestroyDevice(device, callbacksAllocator);
		vkDestroySurfaceKHR(context, surface, callbacksAllocator);
		vkDestroyInstance(context, callbacksAllocator);
	}
	
	void VulkanContext::SwapBuffers()
	{

	}
}