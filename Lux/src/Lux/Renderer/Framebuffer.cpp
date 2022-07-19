#include "luxpch.h"
#include "Framebuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Lux
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLFramebuffer>(specification);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}