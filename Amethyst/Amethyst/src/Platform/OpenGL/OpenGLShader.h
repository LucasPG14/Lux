#pragma once

#include "Amethyst/Renderer/Shader.h"

namespace Amethyst
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertex, const std::string& fragment);
		~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		void UploadUniformInt(const std::string& name, uint32_t value) override;
	private:
		uint32_t shaderID;
	};
}