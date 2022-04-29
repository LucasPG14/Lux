#include "amtpch.h"
#include "VulkanPipeline.h"

#include "VulkanContext.h"

namespace Amethyst
{
	VulkanPipeline::VulkanPipeline() : pipeline(VK_NULL_HANDLE)
	{
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(VulkanContext::GetDevice(), pipeline, VulkanContext::GetCallbackAllocator());
	}

	void VulkanPipeline::Bind()
	{
		// Needs command buffer
		vkCmdBindPipeline(VulkanContext::GetCommandBuffer()[0], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}
}