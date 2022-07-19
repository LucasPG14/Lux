#pragma once

#include "Lux/Renderer/Buffer.h"

namespace Lux
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(Vertex* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;

		inline const BufferLayout& GetLayout() override { return layout; }
		void SetLayout(const BufferLayout& lay) override { layout = lay; }

	private:
		uint32_t vertexID;
		BufferLayout layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t indCount);
		~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		inline uint32_t GetCount() const override { return count; }

	private:
		uint32_t indexID;
		uint32_t count;
	};
}