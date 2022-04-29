#pragma once

#include <vulkan/vulkan.h>

namespace Amethyst
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass();
		~VulkanRenderPass();

		void Begin();
		void End();
	private:
		VkRenderPass renderPass;
	};
}