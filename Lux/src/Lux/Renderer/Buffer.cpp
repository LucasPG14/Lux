#include "luxpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Lux
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLVertexBuffer>(size);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(Vertex* vertices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetRenderer())
		{
			case Render::API::NONE:
			{
				LUX_CORE_ASSERT(false, "There's no RendererAPI");
				return nullptr;
			}
			case Render::API::OPENGL: return std::make_shared<OpenGLIndexBuffer>(indices, size);
		}

		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
}