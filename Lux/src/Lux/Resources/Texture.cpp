#include "luxpch.h"
#include "Texture.h"

#include "Lux/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Lux
{
	// Texture 2D
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLTexture2D>(path);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
	
	std::shared_ptr<Texture2D> Texture2D::Create(const void* data, int width, int height)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLTexture2D>(data, width, height);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}


	// Texture Cube

	std::shared_ptr<TextureCube> TextureCube::Create(const std::vector<std::string>& paths)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLTextureCube>(paths);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}