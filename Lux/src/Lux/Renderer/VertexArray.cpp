#include "luxpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Lux
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLVertexArray>();
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}