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
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	}
}