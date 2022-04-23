#pragma once

#include "Amethyst/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>
struct GLFWwindow;

namespace Amethyst
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* currentWindow);

		void Init() override;
		void Shutdown() override;

		void SwapBuffers() override;

	private:
		GLFWwindow* window;

		VkInstance context;
		VkAllocationCallbacks* callbacksAllocator;
		VkPhysicalDevice physicalDevice;

		int32_t queueCount;
		VkQueue queue;

		VkDevice device;
		VkDescriptorPool descriptorPool;

		VkSurfaceKHR surface;

		VkSwapchainKHR swapChain;
		uint32_t imgCount;
		VkImage* images;
		VkImageView* imageViews;

		VkFormat format;
		VkExtent2D extent;
	};
}