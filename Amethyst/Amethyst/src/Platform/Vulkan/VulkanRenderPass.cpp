#include "amtpch.h"
#include "VulkanRenderPass.h"

#include "VulkanContext.h"

namespace Amethyst
{
	VulkanRenderPass::VulkanRenderPass() : renderPass(VK_NULL_HANDLE)
	{
	}
	
	VulkanRenderPass::~VulkanRenderPass()
	{
		vkDestroyRenderPass(VulkanContext::GetVkDevice().GetDevice(), renderPass, VulkanContext::GetCallbackAllocator());
	}
	
	void VulkanRenderPass::Begin()
	{
	}
	
	void VulkanRenderPass::End()
	{
	}
}