#include "amtpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Amethyst
{
	Shader* Shader::Create(const std::string& vertex, const std::string& fragment)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE: 
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return new OpenGLShader(vertex, fragment);
		}
		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}