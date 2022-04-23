#pragma once

#include "Amethyst/Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Amethyst
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath);
		VulkanShader(const std::string& name, const std::string& vertex, const std::string& fragment);
		~VulkanShader();

		void Bind() const override;
		void Unbind() const override;

		const std::string& GetName() const override { return name; }

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		void UploadUniformInt(const std::string& name, uint32_t value) override;
	private:
		void CompileShaders(const std::string& filePath, const std::string& fragment);

	private:
		VkPipelineShaderStageCreateInfo stages[2];
		VkShaderModule vertexModule;
		VkShaderModule fragmentModule;

		uint32_t shaderID;
		std::string name;
	};
}