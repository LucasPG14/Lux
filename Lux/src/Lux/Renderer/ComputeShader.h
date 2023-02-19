#pragma once

typedef unsigned int GLenum;

namespace Lux
{
	class Texture2D;

	class ComputeShader
	{
	public:
		ComputeShader(const std::string& filePath);
		~ComputeShader();

		void DispatchCompute();

		void Bind() const;
		void Unbind() const;

		void BindTexture(uint32_t slot = 0) const;
		void UnbindTexture(uint32_t slot = 0) const;

		// Uniforms
		void SetUniformInt(const std::string& name, uint32_t value);

		void ResizeTexture(int width, int height);
	private:
		void CompileShader(const std::string& path);
		std::string ReadFile(const std::string& path);

	private:
		uint32_t shaderID;
		std::string name;
		std::shared_ptr<Texture2D> texture;
	};
}