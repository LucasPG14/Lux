#pragma once

#include <glm/glm.hpp>

typedef unsigned int GLenum;

namespace Lux
{
	class Shader
	{
	public:
		Shader(const std::string& filePath);
		Shader(const std::string& name, const std::string& vertex, const std::string& fragment);
		virtual ~Shader();

		void Bind() const;
		void Unbind() const;

		const std::string& GetName() const { return name; }

		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
		void SetUniformInt(const std::string& name, uint32_t value);
		void SetUniformFloat3(const std::string& name, const glm::vec3& value);
		void SetUniformFloat2(const std::string& name, const glm::vec2& value);
		void SetUniformFloat(const std::string& name, float value);

		void SetStorageBlock(const std::string& name, int blockIndexBinding);

	private:
		void CompileShader(const std::unordered_map<GLenum, std::string>& map);
		std::unordered_map<GLenum, std::string> PreProcessShader(const std::string& src);
		std::string ReadFile(const std::string& path);

	private:
		uint32_t shaderID;
		std::string name;
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::shared_ptr<Shader>& shader);
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);

		std::shared_ptr<Shader> Load(const std::string& filepath);
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

		const std::shared_ptr<Shader>& Get(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
	};
}