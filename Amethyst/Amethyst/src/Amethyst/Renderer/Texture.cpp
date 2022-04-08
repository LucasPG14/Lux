#include "amtpch.h"
#include "Texture.h"

#include "Renderer2D.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Amethyst
{
	Texture2D* Texture2D::Create(const std::string& path)
	{
		switch (Renderer2D::GetRenderer())
		{
		case Render::API::NONE:
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return new OpenGLTexture2D(path);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}