
#include "amtpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
{
	OpenGLShader::OpenGLShader(const std::string& vertex, const std::string& fragment) : shaderID(0)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertex.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			AMT_CORE_ERROR("{0}", infoLog.data());
			AMT_CORE_ASSERT(false, "Vertex Shader Compilation Failed!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragment.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			AMT_CORE_ERROR("{0}", infoLog.data());
			AMT_CORE_ASSERT(false, "Fragment Shader Compilation Failed!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		shaderID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(shaderID, vertexShader);
		glAttachShader(shaderID, fragmentShader);

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
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			AMT_CORE_ERROR("{0}", infoLog.data());
			AMT_CORE_ASSERT(false, "Shader Link Failed!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(shaderID, vertexShader);
		glDetachShader(shaderID, fragmentShader);
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

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::transpose(matrix)));
	}
	
	void OpenGLShader::UploadUniformInt(const std::string& name, uint32_t value)
	{
		GLint location = glGetUniformLocation(shaderID, name.c_str());
		glUniform1i(location, value);
	}
}