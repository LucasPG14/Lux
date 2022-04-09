#include "amtpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Amethyst
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return new OpenGLVertexArray();
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}