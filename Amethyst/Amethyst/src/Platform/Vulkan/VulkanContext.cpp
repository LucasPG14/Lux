#include "amtpch.h"
#include "VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Amethyst
{
	static const int maxFramesInFlight = 2;

	// Temporary
	VulkanDevice VulkanContext::device = {};

	VkInstance VulkanContext::context = VK_NULL_HANDLE;
	VkQueue VulkanContext::queue = VK_NULL_HANDLE;
	
	uint32_t VulkanContext::imgCount = 0;
	VkAllocationCallbacks* VulkanContext::callbacksAllocator = VK_NULL_HANDLE;
	VkSurfaceKHR VulkanContext::surface = VK_NULL_HANDLE;
	VkFormat VulkanContext::format = {};

	std::vector<VkImage> VulkanContext::images = {};
	std::vector<VkImageView> VulkanContext::imageViews = {};
	
	VkShaderModule VulkanContext::vertexModule = VK_NULL_HANDLE;
	VkShaderModule VulkanContext::fragmentModule = VK_NULL_HANDLE;

	VkPipelineShaderStageCreateInfo VulkanContext::stages[2] = {};
	
	std::vector<VkCommandBuffer> VulkanContext::commandBuffer = {};
	VkRenderPass VulkanContext::renderPass = VK_NULL_HANDLE;
	VkPipeline VulkanContext::graphicsPipeline = VK_NULL_HANDLE;
	
	VkSwapchainKHR VulkanContext::swapChain = VK_NULL_HANDLE;
	VkPipelineLayout VulkanContext::pipelineLayout = VK_NULL_HANDLE;
	
	std::vector<VkSemaphore> VulkanContext::imageAvailable = {};
	std::vector<VkSemaphore> VulkanContext::renderFinished = {};
	std::vector<VkFence> VulkanContext::inFlightFence = {};
	
	VkExtent2D VulkanContext::extent = {};
	std::vector<VkFramebuffer> VulkanContext::framebuffers = {};

	std::vector<MyVertex> VulkanContext::vertices = {};
	std::vector<uint32_t> VulkanContext::indices = {};

	VkBuffer VulkanContext::vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory VulkanContext::vertexBufferMemory = VK_NULL_HANDLE;
	
	VkCommandPool VulkanContext::commandPool = VK_NULL_HANDLE;

	VkBuffer VulkanContext::stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory VulkanContext::stagingBufferMemory = VK_NULL_HANDLE;

	VkBuffer VulkanContext::indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory VulkanContext::indexBufferMemory = VK_NULL_HANDLE;

	VkDescriptorPool VulkanContext::descriptorPool = VK_NULL_HANDLE;

	int VulkanContext::currentFrame = 0;
	uint32_t VulkanContext::myIndex = 0;

#ifdef _DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
	{
		(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
		fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
		return VK_FALSE;
	}
#endif

	static void CheckVkResult(VkResult error)
	{
		AMT_CORE_ASSERT(!error, "Vulkan error");
	}

	VulkanContext::VulkanContext(GLFWwindow* currentWindow) 
		: window(currentWindow), queueCount(-1)
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

#ifdef _DEBUG
			// Enabling validation layers
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			instanceCreateInfo.enabledLayerCount = 1;
			instanceCreateInfo.ppEnabledLayerNames = layers;

			// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
			const char** extensionsExt = (const char**)malloc(sizeof(const char*) * (extensionsCount + 1));
			memcpy(extensionsExt, extensions, extensionsCount * sizeof(const char*));
			extensionsExt[extensionsCount] = "VK_EXT_debug_report";
			instanceCreateInfo.enabledExtensionCount = extensionsCount + 1;
			instanceCreateInfo.ppEnabledExtensionNames = extensionsExt;

			// Create Vulkan Instance
			error = vkCreateInstance(&instanceCreateInfo, callbacksAllocator, &context);
			CheckVkResult(error);
			free(extensionsExt);

			// Get the function pointer (required for any extensions)
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(context, "vkCreateDebugReportCallbackEXT");

			// Setup the debug report callback
			VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
			debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debugReportCreateInfo.pfnCallback = debug_report;
			debugReportCreateInfo.pUserData = NULL;
			error = vkCreateDebugReportCallbackEXT(context, &debugReportCreateInfo, callbacksAllocator, &debugReport);
			CheckVkResult(error);
#else
			// Create Vulkan Instance without any debug feature
			error = vkCreateInstance(&instanceCreateInfo, callbacksAllocator, &context);
			CheckVkResult(error);
#endif

			// Creating the context of Vulkan and checking for errors
		}

		device.Init();

		queueCount = device.GetQueueCount();
		vkGetDeviceQueue(device.GetDevice(), queueCount, 0, &queue);

		// -------------- Setting the descriptor pools --------------
		{

		}

		// ------------------ Creating the surface ------------------
		{
			error = glfwCreateWindowSurface(context, window, callbacksAllocator, &surface);
			CheckVkResult(error);
		}

		// Setting the command pool
		{
			VkCommandPoolCreateInfo commandPoolInfo = {};
			commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolInfo.queueFamilyIndex = queueCount;

			error = vkCreateCommandPool(device.GetDevice(), &commandPoolInfo, callbacksAllocator, &commandPool);
			CheckVkResult(error);

			commandBuffer.resize(maxFramesInFlight);

			// Setting command buffer
			VkCommandBufferAllocateInfo cmdAllocInfo = {};
			cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdAllocInfo.commandPool = commandPool;
			cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmdAllocInfo.commandBufferCount = commandBuffer.size();

			error = vkAllocateCommandBuffers(device.GetDevice(), &cmdAllocInfo, commandBuffer.data());
			CheckVkResult(error);
		}

		CreateSwapChain();

		// Setting sync objects
		{
			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			imageAvailable.resize(maxFramesInFlight);
			renderFinished.resize(maxFramesInFlight);
			inFlightFence.resize(maxFramesInFlight);

			for (int i = 0; i < maxFramesInFlight; ++i)
			{
				vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, callbacksAllocator, &imageAvailable[i]);
				vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, callbacksAllocator, &renderFinished[i]);
				vkCreateFence(device.GetDevice(), &fenceInfo, callbacksAllocator, &inFlightFence[i]);
			}
		}
	}

	void VulkanContext::Shutdown()
	{
		vkDeviceWaitIdle(device.GetDevice());
		
		CleanUpSwapChain();

		vkDestroyBuffer(device.GetDevice(), indexBuffer, callbacksAllocator);
		vkFreeMemory(device.GetDevice(), indexBufferMemory, callbacksAllocator);

		vkDestroyBuffer(device.GetDevice(), vertexBuffer, callbacksAllocator);
		vkFreeMemory(device.GetDevice(), vertexBufferMemory, callbacksAllocator);

		for (int i = 0; i < maxFramesInFlight; ++i)
		{
			vkDestroySemaphore(device.GetDevice(), renderFinished[i], callbacksAllocator);
			vkDestroySemaphore(device.GetDevice(), imageAvailable[i], callbacksAllocator);
			vkDestroyFence(device.GetDevice(), inFlightFence[i], callbacksAllocator);
		}

		vkDestroyCommandPool(device.GetDevice(), commandPool, callbacksAllocator);
		
		vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, callbacksAllocator);
		device.Shutdown();
		vkDestroySurfaceKHR(context, surface, callbacksAllocator);

#ifdef _DEBUG
		//vkDestroyDebugReportCallbackEXT(context, debugReport, callbacksAllocator);
#endif

		vkDestroyInstance(context, callbacksAllocator);
	}
	
	void VulkanContext::Draw()
	{
		vkCmdBindPipeline(commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkDeviceSize offsets = 0;
		vkCmdBindVertexBuffers(commandBuffer[currentFrame], 0, 1, &vertexBuffer, &offsets);

		vkCmdBindIndexBuffer(commandBuffer[currentFrame], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer[currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void VulkanContext::SwapBuffers()
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailable[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinished[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkQueueSubmit(queue, 1, &submitInfo, inFlightFence[currentFrame]);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapChain;
		presentInfo.pImageIndices = &myIndex;

		presentInfo.pResults = nullptr;

		VkResult error = vkQueuePresentKHR(queue, &presentInfo);

		if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapChain();
		}

		currentFrame = (currentFrame + 1) % maxFramesInFlight;
	}

	void VulkanContext::Begin()
	{
		vkWaitForFences(device.GetDevice(), 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

		vkResetFences(device.GetDevice(), 1, &inFlightFence[currentFrame]);

		VkResult error = vkAcquireNextImageKHR(device.GetDevice(), swapChain, UINT64_MAX, imageAvailable[currentFrame], VK_NULL_HANDLE, &myIndex);

		if (error == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		vkResetCommandBuffer(commandBuffer[currentFrame], 0);

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = 0;
		cmdBeginInfo.pInheritanceInfo = nullptr;

		error = vkBeginCommandBuffer(commandBuffer[currentFrame], &cmdBeginInfo);
		CheckVkResult(error);
	}

	void VulkanContext::End()
	{
	}

	void VulkanContext::BeginRenderPass()
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffers[myIndex];

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = extent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer[currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanContext::EndRenderPass()
	{
		vkCmdEndRenderPass(commandBuffer[currentFrame]);

		VkResult error = vkEndCommandBuffer(commandBuffer[currentFrame]);
		CheckVkResult(error);
	}

	void VulkanContext::RecreateSwapChain()
	{
		vkDeviceWaitIdle(device.GetDevice());

		CleanUpSwapChain();

		CreateSwapChain();
	}
	
	void VulkanContext::CompileShaders(const std::string& vertex, const std::string& fragment)
	{
		// Vertex file
		std::ifstream vertexFile(vertex, std::ios::ate | std::ios::binary);

		size_t fileSize = (size_t)vertexFile.tellg();
		std::vector<char> vertBuffer(fileSize);

		vertexFile.seekg(0);
		vertexFile.read(vertBuffer.data(), fileSize);

		vertexFile.close();

		VkShaderModuleCreateInfo vertCreateInfo = {};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertBuffer.size();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertBuffer.data());

		VkResult error = vkCreateShaderModule(device.GetDevice(), &vertCreateInfo, callbacksAllocator, &vertexModule);
		CheckVkResult(error);

		// Fragment file
		std::ifstream fragFile(fragment, std::ios::ate | std::ios::binary);

		fileSize = (size_t)fragFile.tellg();
		std::vector<char> fragBuffer(fileSize);

		fragFile.seekg(0);
		fragFile.read(fragBuffer.data(), fileSize);

		fragFile.close();

		VkShaderModuleCreateInfo fragCreateInfo = {};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragBuffer.size();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragBuffer.data());

		error = vkCreateShaderModule(device.GetDevice(), &fragCreateInfo, callbacksAllocator, &fragmentModule);
		CheckVkResult(error);
	}
	
	void VulkanContext::RecordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t index)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		//vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void VulkanContext::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory)
	{
		// Creating the vertex buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult error = vkCreateBuffer(device.GetDevice(), &bufferInfo, callbacksAllocator, &buffer);
		CheckVkResult(error);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device.GetDevice(), buffer, &memRequirements);

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProperties);

		int mem = 0;
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (memRequirements.memoryTypeBits & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				mem = i;
				break;
			}
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = mem;

		error = vkAllocateMemory(device.GetDevice(), &allocInfo, callbacksAllocator, &memory);
		CheckVkResult(error);

		error = vkBindBufferMemory(device.GetDevice(), buffer, memory, 0);
		CheckVkResult(error);
	}

	void VulkanContext::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocIndexInfo = {};
		allocIndexInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocIndexInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocIndexInfo.commandPool = commandPool;
		allocIndexInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VkResult error = vkAllocateCommandBuffers(device.GetDevice(), &allocIndexInfo, &commandBuffer);
		CheckVkResult(error);

		VkCommandBufferBeginInfo beginIndexInfo = {};
		beginIndexInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginIndexInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginIndexInfo);

		VkBufferCopy copyIndexRegion = {};
		copyIndexRegion.srcOffset = 0; // Optional
		copyIndexRegion.dstOffset = 0; // Optional
		copyIndexRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyIndexRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitIndexInfo = {};
		submitIndexInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitIndexInfo.commandBufferCount = 1;
		submitIndexInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(queue, 1, &submitIndexInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(device.GetDevice(), commandPool, 1, &commandBuffer);
	}
	
	void VulkanContext::CreateSwapChain()
	{
		// ----------------- Creating the swapchain -----------------
		{
			// Getting surface capabilities
			VkSurfaceCapabilitiesKHR capabilities;
			VkResult error = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), surface, &capabilities);
			CheckVkResult(error);

			extent = capabilities.currentExtent;

			// Getting surface formats
			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &formatCount, NULL);

			AMT_CORE_ASSERT(formatCount != 0, "There aren't any surface formats");

			std::vector<VkSurfaceFormatKHR> surfaceFormats;
			surfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &formatCount, surfaceFormats.data());

			// Getting surface present modes
			uint32_t presentCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &presentCount, NULL);

			AMT_CORE_ASSERT(presentCount != 0, "There aren't any surface present modes");

			std::vector<VkPresentModeKHR> presentModes;
			presentModes.resize(presentCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &presentCount, presentModes.data());

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
			error = vkCreateSwapchainKHR(device.GetDevice(), &swapChainCreateInfo, callbacksAllocator, &swapChain);
			CheckVkResult(error);

			// Getting the swapchain images
			vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imgCount, NULL);
			images.resize(imgCount);
			error = vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imgCount, images.data());
			CheckVkResult(error);

			// Creating the image view for the swapchain
			imageViews.resize(imgCount);
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

				error = vkCreateImageView(device.GetDevice(), &imgViewCreateInfo, callbacksAllocator, &imageViews[i]);
				CheckVkResult(error);
			}

			// Creating the shader
			{
				CompileShaders("assets/shaders/VulkanVert.spv", "assets/shaders/VulkanFrag.spv");

				VkPipelineShaderStageCreateInfo vertexCreateInfo = {};
				vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertexCreateInfo.module = vertexModule;
				vertexCreateInfo.pName = "main";

				VkPipelineShaderStageCreateInfo fragmentCreateInfo = {};
				fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragmentCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragmentCreateInfo.module = fragmentModule;
				fragmentCreateInfo.pName = "main";

				stages[0] = vertexCreateInfo;
				stages[1] = fragmentCreateInfo;
			}

			// Setting render pass
			{
				VkAttachmentDescription attachment = {};
				attachment.format = format;
				attachment.samples = VK_SAMPLE_COUNT_1_BIT;

				attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

				attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference colorAttachmentReference = {};
				colorAttachmentReference.attachment = 0;
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpassDescription = {};
				subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;


				subpassDescription.colorAttachmentCount = 1;
				subpassDescription.pColorAttachments = &colorAttachmentReference;

				VkRenderPassCreateInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = 1;
				renderPassInfo.pAttachments = &attachment;
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpassDescription;

				VkSubpassDependency subPassDependency = {};
				subPassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				subPassDependency.dstSubpass = 0;

				subPassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				subPassDependency.srcAccessMask = 0;

				subPassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				subPassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &subPassDependency;

				error = vkCreateRenderPass(device.GetDevice(), &renderPassInfo, callbacksAllocator, &renderPass);
				CheckVkResult(error);
			}

			// Setting the pipeline
			{
				// Setting the vertex input
				vertices =
				{
					{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
					{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
					{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
					{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
				};

				indices =
				{
					0, 1, 2, 2, 3, 0
				};

				VkVertexInputBindingDescription bindingDescription = {};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(MyVertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(MyVertex, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(MyVertex, color);

				VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

				VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
				CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

				void* data;
				vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, vertices.data(), (size_t)bufferSize);
				vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

				bufferSize = sizeof(vertices[0]) * vertices.size();
				CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

				CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

				vkDestroyBuffer(device.GetDevice(), stagingBuffer, callbacksAllocator);
				vkFreeMemory(device.GetDevice(), stagingBufferMemory, callbacksAllocator);

				// ----------------------------- Index Buffer ------------------------------------
				bufferSize = sizeof(indices[0]) * indices.size();

				VkBuffer stagingIndexBuffer;
				VkDeviceMemory stagingIndexBufferMemory;
				CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingIndexBuffer, stagingIndexBufferMemory);

				data = nullptr;
				vkMapMemory(device.GetDevice(), stagingIndexBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, indices.data(), (size_t)bufferSize);
				vkUnmapMemory(device.GetDevice(), stagingIndexBufferMemory);

				CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

				CopyBuffer(stagingIndexBuffer, indexBuffer, bufferSize);

				vkDestroyBuffer(device.GetDevice(), stagingIndexBuffer, callbacksAllocator);
				vkFreeMemory(device.GetDevice(), stagingIndexBufferMemory, callbacksAllocator);

				//void* data;
				//vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
				//memcpy(data, vertices.data(), (size_t)bufferSize);
				//vkUnmapMemory(device, vertexBufferMemory);

				// Setting the input assembly
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
				inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

				// Viewports and scissors
				VkViewport viewport = {};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)extent.width;
				viewport.height = (float)extent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				VkRect2D scissors = {};
				scissors.offset = { 0,0 };
				scissors.extent = extent;

				VkPipelineViewportStateCreateInfo viewportInfo = {};
				viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportInfo.viewportCount = 1;
				viewportInfo.pViewports = &viewport;
				viewportInfo.scissorCount = 1;
				viewportInfo.pScissors = &scissors;

				// Setting the rasterizer
				VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
				rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizerInfo.depthClampEnable = VK_FALSE;
				rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
				rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizerInfo.lineWidth = 1.0f;
				rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

				rasterizerInfo.depthBiasEnable = VK_FALSE;
				rasterizerInfo.depthBiasConstantFactor = 0.0f;
				rasterizerInfo.depthBiasClamp = 0.0f;
				rasterizerInfo.depthBiasSlopeFactor = 0.0f;

				// Setting the multisampling
				VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
				multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisamplingInfo.sampleShadingEnable = VK_FALSE;
				multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				multisamplingInfo.minSampleShading = 1.0f;
				multisamplingInfo.pSampleMask = nullptr;
				multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
				multisamplingInfo.alphaToOneEnable = VK_FALSE;

				// Setting the color attachment
				VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
				colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachment.blendEnable = VK_FALSE;
				colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
				colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
				colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
				colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

				VkPipelineColorBlendStateCreateInfo colorBlending = {};
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachment;
				colorBlending.blendConstants[0] = 0.0f;
				colorBlending.blendConstants[1] = 0.0f;
				colorBlending.blendConstants[2] = 0.0f;
				colorBlending.blendConstants[3] = 0.0f;

				// Setting dynamic state
				//std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

				//VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
				//dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				//dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
				//dynamicStateInfo.pDynamicStates = dynamicStates.data();

				// Setting pipeline layout
				VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 0;
				pipelineLayoutInfo.pSetLayouts = nullptr;
				pipelineLayoutInfo.pushConstantRangeCount = 0;
				pipelineLayoutInfo.pPushConstantRanges = nullptr;

				error = vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, callbacksAllocator, &pipelineLayout);
				CheckVkResult(error);

				// Finally creating the pipeline
				VkGraphicsPipelineCreateInfo pipelineInfo = {};
				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;
				pipelineInfo.pStages = stages;
				pipelineInfo.pVertexInputState = &vertexInputInfo;
				pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
				pipelineInfo.pViewportState = &viewportInfo;
				pipelineInfo.pRasterizationState = &rasterizerInfo;
				pipelineInfo.pMultisampleState = &multisamplingInfo;
				pipelineInfo.pDepthStencilState = nullptr;
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.pDynamicState = nullptr;

				pipelineInfo.layout = pipelineLayout;
				pipelineInfo.renderPass = renderPass;
				pipelineInfo.subpass = 0;

				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
				pipelineInfo.basePipelineIndex = -1;

				error = vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
				CheckVkResult(error);

				vkDestroyShaderModule(device.GetDevice(), vertexModule, callbacksAllocator);
				vkDestroyShaderModule(device.GetDevice(), fragmentModule, callbacksAllocator);
			}
		}

		// Setting the swapchain framebuffers
		{
			framebuffers.resize(imageViews.size());
			for (size_t i = 0; i < imageViews.size(); ++i)
			{
				VkImageView attachments[] = { imageViews[i] };

				VkFramebufferCreateInfo fboCreateInfo = {};
				fboCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fboCreateInfo.renderPass = renderPass;
				fboCreateInfo.attachmentCount = 1;
				fboCreateInfo.pAttachments = attachments;
				fboCreateInfo.width = extent.width;
				fboCreateInfo.height = extent.height;
				fboCreateInfo.layers = 1;

				VkResult error = vkCreateFramebuffer(device.GetDevice(), &fboCreateInfo, callbacksAllocator, &framebuffers[i]);
				CheckVkResult(error);
			}
		}
	}

	void VulkanContext::CleanUpSwapChain()
	{
		for (size_t i = 0; i < framebuffers.size(); ++i)
		{
			vkDestroyFramebuffer(device.GetDevice(), framebuffers[i], callbacksAllocator);
		}

		vkDestroyPipeline(device.GetDevice(), graphicsPipeline, callbacksAllocator);
		vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, callbacksAllocator);
		vkDestroyRenderPass(device.GetDevice(), renderPass, callbacksAllocator);

		for (size_t i = 0; i < imageViews.size(); ++i)
		{
			vkDestroyImageView(device.GetDevice(), imageViews[i], callbacksAllocator);
		}

		vkDestroySwapchainKHR(device.GetDevice(), swapChain, callbacksAllocator);
	}
}