#pragma once

namespace Amethyst
{
	class Shader
	{
	public:
		Shader(const std::string& vertex, const std::string& fragment);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t shaderID;
	};
}