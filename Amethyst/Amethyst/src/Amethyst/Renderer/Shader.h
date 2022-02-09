#pragma once

#include <glm/glm.hpp>

namespace Amethyst
{
	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void UploadUniformInt(const std::string& name, uint32_t value) = 0;
		
		static Shader* Create(const std::string& vertex, const std::string& fragment);
	};
}