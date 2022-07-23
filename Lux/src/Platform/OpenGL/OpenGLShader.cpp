#include "luxpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	static GLenum ShaderTypeFromString(const std::string& string)
	{
		if (string == "vertex") return GL_VERTEX_SHADER;
		if (string == "fragment") return GL_FRAGMENT_SHADER;
	
		LUX_CORE_ASSERT(false, "This shader doesn't exist!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath) : shaderID(0)
	{
		std::string shaderSrc = ReadFile(filePath);
		auto sources = PreProcessShader(shaderSrc);
		CompileShader(sources);

		std::filesystem::path path(filePath);
		name = path.stem().string();
	}

	OpenGLShader::OpenGLShader(const std::string& shaderName, const std::string& vertex, const std::string& fragment) : shaderID(0), name(shaderName)
	{
		std::unordered_map<GLenum, std::string> srcs;

		srcs[GL_VERTEX_SHADER] = vertex;
		srcs[GL_FRAGMENT_SHADER] = fragment;
		CompileShader(srcs);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(0);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(shaderID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	
	void OpenGLShader::SetUniformInt(const std::string& name, uint32_t value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform1f(location, value);
	}
	
	void OpenGLShader::CompileShader(const std::unordered_map<GLenum, std::string>& map)
	{
		GLuint program = glCreateProgram();

		LUX_CORE_ASSERT(map.size() <= 2, "More than 2 shaders not supported!");
		std::array<GLenum, 2> shadersIDs;

		int index = 0;
		for (auto& src : map)
		{
			GLenum type = src.first;
			const std::string& shaderSrc = src.second;
			
			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* source = shaderSrc.c_str();
			glShaderSource(shader, 1, &source, 0);

			// Compile the vertex shader
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
				break;
			}

			glAttachShader(program, shader);
			shadersIDs[index] = shader;
			index++;
		}

		shaderID = program;
		// Link our program
		glLinkProgram(shaderID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
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

			for (int i = 0; i < shadersIDs.size(); ++i)
				glDeleteShader(shadersIDs[i]);

			LUX_CORE_ERROR("{0}", infoLog.data());
			LUX_CORE_ASSERT(false, "Shader Link Failed!");
			return;
		}

		// Always detach shaders after a successful link.
		for (int i = 0; i < shadersIDs.size(); ++i)
			glDetachShader(shaderID, shadersIDs[i]);
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcessShader(const std::string& src)
	{
		std::unordered_map<GLenum, std::string> shaders;

		const char* type = "#type";
		size_t typeLength = strlen(type);
		size_t pos = src.find(type, 0);

		while (pos != std::string::npos)
		{
			size_t eol = src.find_first_of("\r\n", pos);
			LUX_CORE_ASSERT(eol != std::string::npos, "Syntax Error");
			size_t begin = pos + typeLength + 1;
			std::string shader = src.substr(begin, eol - begin);
			
			LUX_CORE_ASSERT(shader == "vertex" || shader == "fragment", "Invalid Shader Type");

			size_t nextLinePosition = src.find_first_not_of("\r\n", eol);
			pos = src.find(type, nextLinePosition);
			shaders[ShaderTypeFromString(shader)] = src.substr(nextLinePosition, pos - (nextLinePosition == std::string::npos ? src.size() - 1 : nextLinePosition));
		}

		return shaders;
	}
	
	std::string OpenGLShader::ReadFile(const std::string& path)
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