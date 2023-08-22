#include "luxpch.h"

#include "ShaderStorageBuffer.h"
#include <glad/glad.h>

namespace Lux
{
	ShaderStorageBuffer::ShaderStorageBuffer(const void* data, uint32_t size, uint32_t binding) : bindingIndex(binding)
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	
	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
	}
	
	void ShaderStorageBuffer::Bind()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
	}

	void ShaderStorageBuffer::Reset(const void* data, uint32_t size)
	{
		if (ssbo >= 0)
		{
			glDeleteBuffers(1, &ssbo);
		}

		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void ShaderStorageBuffer::ChangeData(void* data, uint32_t offset, uint32_t size)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
		//GLenum error = glGetError();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}