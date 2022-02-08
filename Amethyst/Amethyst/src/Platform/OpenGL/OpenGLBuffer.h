#pragma once

#include "Amethyst/Renderer/Buffer.h"

namespace Amethyst
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;

	private:
		uint32_t vertexID;
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