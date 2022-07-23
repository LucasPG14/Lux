#include "luxpch.h"
#include "VertexBuffer.h"

#include <glad/glad.h>

namespace Lux
{
	VertexBuffer::VertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(Vertex* vertices, uint32_t size)
	{
		glCreateBuffers(1, &vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	void VertexBuffer::Bind() const
	{
	}

	void VertexBuffer::Unbind() const
	{
	}

	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
	}
}