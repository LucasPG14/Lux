#include "luxpch.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Lux
{
	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t size) : count(size)
	{
		glCreateBuffers(1, &indexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &indexID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}