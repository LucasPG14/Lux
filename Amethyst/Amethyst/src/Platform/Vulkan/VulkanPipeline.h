#pragma once

#include <vulkan/vulkan.h>

namespace Amethyst
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		void Bind();
		void Unbind();
	private:
		VkPipeline pipeline;
	};
}