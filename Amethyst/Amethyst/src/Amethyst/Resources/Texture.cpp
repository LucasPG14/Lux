#include "amtpch.h"
#include "Texture.h"

#include "Amethyst/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Amethyst
{
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLTexture2D>(path);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}