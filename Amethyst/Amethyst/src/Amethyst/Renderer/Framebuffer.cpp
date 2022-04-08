#include "amtpch.h"
#include "Framebuffer.h"

#include "Renderer2D.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Amethyst
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer2D::GetRenderer())
		{
		case Render::API::NONE:
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLFramebuffer>(specification);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}