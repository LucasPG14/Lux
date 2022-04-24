#include "amtpch.h"
#include "VulkanRender.h"

#include <vulkan/vulkan.h>

namespace Amethyst
{
	void VulkanRender::Init() const
	{
	}

	void VulkanRender::ClearColor(const glm::vec4& color) const
	{
		VkClearValue colorValue = { color.r, color.g, color.b, color.a };

	}

	void VulkanRender::Clear() const
	{
	}

	void VulkanRender::ChangeState(bool change) const
	{
	}

	void VulkanRender::Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		
	}

}