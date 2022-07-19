#pragma once

#include "Amethyst/Renderer/Shader.h"
#include <glad/glad.h>

namespace Amethyst
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertex, const std::string& fragment);
		~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		const std::string& GetName() const override { return name; }

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		void UploadUniformInt(const std::string& name, uint32_t value) override;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value) override;
		void UploadUniformFloat(const std::string& name, float value) override;
	private:
		void CompileShader(const std::unordered_map<GLenum, std::string>& map);
		std::unordered_map<GLenum, std::string> PreProcessShader(const std::string& src);
		std::string ReadFile(const std::string& path);
	private:
		uint32_t shaderID;
		std::string name;
	};
}