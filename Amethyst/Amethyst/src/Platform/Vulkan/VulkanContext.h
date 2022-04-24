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
		
		// Temporary
		static void Draw();

		void SwapBuffers() override;


	private:
		void CompileShaders(const std::string& vertex, const std::string& fragment);
		static void RecordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t index);

	private:
		GLFWwindow* window;

		VkInstance context;
		VkAllocationCallbacks* callbacksAllocator;
		VkPhysicalDevice physicalDevice;

		int32_t queueCount;
		static VkQueue queue;

		static VkDevice device;
		VkDescriptorPool descriptorPool;

		VkSurfaceKHR surface;

		static VkSwapchainKHR swapChain;
		uint32_t imgCount;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;

		VkPipelineShaderStageCreateInfo stages[2];
		VkShaderModule vertexModule;
		VkShaderModule fragmentModule;

		VkFormat format;
		static VkExtent2D extent;

		VkPipelineLayout pipelineLayout;
		static VkRenderPass renderPass;
		static VkPipeline graphicsPipeline;

		static std::vector<VkFramebuffer> framebuffers;

		VkCommandPool commandPool;
		static VkCommandBuffer commandBuffer;

		static VkSemaphore imageAvailable;
		static VkSemaphore renderFinished;
		static VkFence inFlightFence;
	};
}