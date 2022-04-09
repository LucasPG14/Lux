#include "amtpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Amethyst
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
			case Render::API::NONE: 
			{
				AMT_CORE_ASSERT(false, "There's no RendererAPI");
				return nullptr;
			}
			case Render::API::OPENGL: return new OpenGLVertexBuffer(vertices, size);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Create(Vertex* vertices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			AMT_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return new OpenGLVertexBuffer(vertices, size);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
			case Render::API::NONE:
			{
				AMT_CORE_ASSERT(false, "There's no RendererAPI");
				return nullptr;
			}
			case Render::API::OPENGL: return new OpenGLIndexBuffer(indices, size);
		}

		AMT_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}