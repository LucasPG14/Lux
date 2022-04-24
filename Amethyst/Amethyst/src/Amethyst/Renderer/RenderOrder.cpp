#include "amtpch.h"
#include "RenderOrder.h"

#include "Platform/OpenGL/OpenGLRender.h"
#include "Platform/Vulkan/VulkanRender.h"

namespace Amethyst
{
	Render* RenderOrder::render = new VulkanRender;
}