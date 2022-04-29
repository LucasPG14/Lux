#pragma once

#include "Amethyst/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
struct GLFWwindow;

namespace Amethyst
{
	struct MyVertex
	{
		glm::vec2 position;
		glm::vec3 color;
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* currentWindow);

		void Init() override;
		void Shutdown() override;
		
		// Temporary
		static void Draw();

		void SwapBuffers() override;

		static void Begin();
		static void End();

		static void BeginRenderPass();
		static void EndRenderPass();

		static void RecreateSwapChain();

		static VkInstance GetInstance() { return context; }
		static VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		static VkAllocationCallbacks* GetCallbackAllocator() { return callbacksAllocator; }
		static VkDevice GetDevice() { return device; }
		static VkSurfaceKHR GetSurface() { return surface; }
		
		static VkExtent2D GetExtent() { return extent; }
		static VkCommandPool GetCommandPool() { return commandPool; }

		static VkFormat GetFormat() { return format; }
		
		static VkQueue GetQueue() { return queue; }
		static VkRenderPass GetRenderPass() { return renderPass; }
		
		static VkDescriptorPool GetDescriptorPool() { return descriptorPool; }

		static int GetCurrentFrame() { return currentFrame; }
		
		static std::vector<VkCommandBuffer> GetCommandBuffer() { return commandBuffer; }
		static VkCommandBuffer GetSpecCommandBuffer() { return commandBuffer[currentFrame]; }

		static uint32_t GetImageCount() { return imgCount; }
	private:
		static void CompileShaders(const std::string& vertex, const std::string& fragment);
		static void RecordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t index);

		static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory);

		static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		static void CreateSwapChain();
		static void CleanUpSwapChain();
	private:
		GLFWwindow* window;

		static VkInstance context;
		VkDebugReportCallbackEXT debugReport;

		static VkAllocationCallbacks* callbacksAllocator;
		static VkPhysicalDevice physicalDevice;

		int32_t queueCount;
		static VkQueue queue;

		static VkDevice device;
		static VkDescriptorPool descriptorPool;

		static VkSurfaceKHR surface;

		static VkSwapchainKHR swapChain;
		static uint32_t imgCount;
		static std::vector<VkImage> images;
		static std::vector<VkImageView> imageViews;

		static VkPipelineShaderStageCreateInfo stages[2];
		static VkShaderModule vertexModule;
		static VkShaderModule fragmentModule;

		static VkFormat format;
		static VkExtent2D extent;

		static VkPipelineLayout pipelineLayout;
		static VkRenderPass renderPass;
		static VkPipeline graphicsPipeline;

		static std::vector<VkFramebuffer> framebuffers;

		static VkCommandPool commandPool;
		static std::vector<VkCommandBuffer> commandBuffer;

		static std::vector<VkSemaphore> imageAvailable;
		static std::vector<VkSemaphore> renderFinished;
		static std::vector<VkFence> inFlightFence;

		static std::vector<MyVertex> vertices;
		static std::vector<uint32_t> indices;

		static VkBuffer vertexBuffer; 
		static VkDeviceMemory vertexBufferMemory;		

		static VkBuffer stagingBuffer;
		static VkDeviceMemory stagingBufferMemory;
		
		static VkBuffer indexBuffer; 
		static VkDeviceMemory indexBufferMemory;

		static int currentFrame;
		static uint32_t myIndex;
	};
}