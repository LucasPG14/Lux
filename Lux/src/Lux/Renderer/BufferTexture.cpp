#include "luxpch.h"
#include "BufferTexture.h"

#include <glad/glad.h>

namespace Lux
{
	BufferTexture::BufferTexture(void* data, uint32_t size) : actualSize(size)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_TEXTURE_BUFFER, buffer);
		glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_BUFFER, texture);
		glTextureBuffer(GL_TEXTURE_BUFFER, GL_RGB16F, buffer);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}
	
	BufferTexture::~BufferTexture()
	{
	}
	
	void BufferTexture::AddData(void* data, uint32_t size)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, buffer);
		glBufferSubData(GL_TEXTURE_BUFFER, actualSize, size, data);
		glBindTexture(GL_TEXTURE_BUFFER, texture);
		glTextureBufferRange(GL_TEXTURE_BUFFER, GL_RGB16F, buffer, actualSize, size);
		glBindTexture(GL_TEXTURE_BUFFER, 0);

		actualSize += size;
	}
}