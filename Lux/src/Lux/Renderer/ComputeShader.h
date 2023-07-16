#pragma once

#include <glm/glm.hpp>

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

		void SetStorageBlock(const std::string& name, int blockIndexBinding);

		uint32_t GetID();

		void SaveToFile();

		// Uniforms
		void SetUniformInt(const std::string& name, uint32_t value);
		void SetUniformFloat2(const std::string& name, const glm::vec2& value);
		void SetUniformFloat3(const std::string& name, const glm::vec3& value);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

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