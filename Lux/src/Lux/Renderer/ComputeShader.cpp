#include "luxpch.h"
#include "ComputeShader.h"

#include "Lux/Resources/Texture2D.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	ComputeShader::ComputeShader(const std::string& filePath)
	{
		std::string shaderSrc = ReadFile(filePath);
		CompileShader(shaderSrc);

		std::filesystem::path path(filePath);
		name = path.stem().string();

		texture = CreateSharedPtr<Texture2D>(512, 512);
	}

	ComputeShader::~ComputeShader()
	{
	}

	void ComputeShader::DispatchCompute()
	{
		texture->BindImage();
		glDispatchCompute(texture->GetWidth(), texture->GetHeight(), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		texture->UnbindImage();
	}

	void ComputeShader::Bind() const
	{
		glUseProgram(shaderID);
	}

	void ComputeShader::Unbind() const
	{
		glUseProgram(0);
	}

	void ComputeShader::BindTexture(uint32_t slot) const
	{
		texture->Bind(slot);
	}

	void ComputeShader::UnbindTexture(uint32_t slot) const
	{
		texture->Bind(slot);
	}

	void ComputeShader::SetStorageBlock(const std::string& name, int blockIndexBinding)
	{
		GLuint blockIndex = 0;
		blockIndex = glGetProgramResourceIndex(shaderID, GL_SHADER_STORAGE_BLOCK, name.c_str());
		glShaderStorageBlockBinding(shaderID, blockIndex, blockIndexBinding);
	}

	uint32_t ComputeShader::GetID()
	{
		return texture->GetID();
	}

	void ComputeShader::SaveToFile()
	{
		texture->SaveToFile();
	}

	void ComputeShader::SetUniformInt(const std::string& name, uint32_t value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform1i(location, value);
	}

	void ComputeShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void ComputeShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void ComputeShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void ComputeShader::ResizeTexture(int width, int height)
	{
		texture->Resize(width, height);
	}
	
	void ComputeShader::CompileShader(const std::string& path)
	{
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		const char* shaderSource = path.c_str();
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shader);

			LUX_CORE_ERROR("{0}", infoLog.data());
			LUX_CORE_ASSERT(false, "Shader Compilation Failed!");
			return;
		}

		shaderID = glCreateProgram();
		glAttachShader(shaderID, shader);
		glLinkProgram(shaderID);
		glValidateProgram(shaderID);

		GLint isLinked = 0;
		glGetProgramiv(shaderID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(shaderID);
			// Don't leak shaders either.

			glDeleteShader(shader);

			LUX_CORE_ERROR("{0}", infoLog.data());
			LUX_CORE_ASSERT(false, "Shader Link Failed!");
			return;
		}

		LUX_CORE_INFO("Compute shader loaded correctly!");

		// Always detach shaders after a successful link.
		glDetachShader(shaderID, shader);
	}
	
	std::string ComputeShader::ReadFile(const std::string& path)
	{
		std::string result;
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else LUX_CORE_ERROR("Couldn't open file '{0}'", path);

		return result;
	}
}