#pragma once

#include "Buffer.h"

namespace Lux
{
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size);
		VertexBuffer(float* vertices, uint32_t size);
		VertexBuffer(Vertex* vertices, uint32_t size);
		virtual ~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size);

		const BufferLayout& GetLayout() { return layout; }
		void SetLayout(const BufferLayout& lay) { layout = lay; }

	private:
		uint32_t vertexID;
		BufferLayout layout;
	};
}