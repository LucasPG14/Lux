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

		virtual const std::string& GetName() const = 0;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void UploadUniformInt(const std::string& name, uint32_t value) = 0;
		
		static std::shared_ptr<Shader> Create(const std::string& filePath);
		static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertex, const std::string& fragment);
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