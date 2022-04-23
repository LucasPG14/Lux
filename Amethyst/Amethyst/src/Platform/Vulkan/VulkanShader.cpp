#include "amtpch.h"
#include "VulkanShader.h"

#include "VulkanManager.h"

namespace Amethyst
{
	VulkanShader::VulkanShader(const std::string& filePath) : shaderID(0)
	{
		std::filesystem::path path(filePath);
		name = path.stem().string();
	}

	VulkanShader::VulkanShader(const std::string& n, const std::string& vertex, const std::string& fragment) : shaderID(0), name(n)
	{
		CompileShaders(vertex, fragment);

		VkPipelineShaderStageCreateInfo vertexCreateInfo = {};
		vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexCreateInfo.module = vertexModule;
		vertexCreateInfo.pName = name.c_str();

		VkPipelineShaderStageCreateInfo fragmentCreateInfo = {};
		vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		vertexCreateInfo.module = fragmentModule;
		vertexCreateInfo.pName = name.c_str();

		stages[0] = vertexCreateInfo;
		stages[1] = fragmentCreateInfo;
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(VulkanManager::GetDevice(), fragmentModule, VulkanManager::GetCallbackAllocator());
		vkDestroyShaderModule(VulkanManager::GetDevice(), vertexModule, VulkanManager::GetCallbackAllocator());
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
	}

	void VulkanShader::UploadUniformInt(const std::string& name, uint32_t value)
	{
	}

	void VulkanShader::CompileShaders(const std::string& vertex, const std::string& fragment)
	{
		// Vertex file
		std::ifstream vertexFile(vertex, std::ios::ate | std::ios::binary);

		size_t fileSize = (size_t)vertexFile.tellg();
		std::vector<char> vertBuffer(fileSize);

		vertexFile.seekg(0);
		vertexFile.read(vertBuffer.data(), fileSize);

		vertexFile.close();

		VkShaderModuleCreateInfo vertCreateInfo = {};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertBuffer.size();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertBuffer.data());

		VkResult error = vkCreateShaderModule(VulkanManager::GetDevice(), &vertCreateInfo, VulkanManager::GetCallbackAllocator(), &vertexModule);

		// Fragment file
		std::ifstream fragFile(vertex, std::ios::ate | std::ios::binary);

		fileSize = (size_t)fragFile.tellg();
		std::vector<char> fragBuffer(fileSize);

		fragFile.seekg(0);
		fragFile.read(fragBuffer.data(), fileSize);

		fragFile.close();

		VkShaderModuleCreateInfo fragCreateInfo = {};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragBuffer.size();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragBuffer.data());

		error = vkCreateShaderModule(VulkanManager::GetDevice(), &fragCreateInfo, VulkanManager::GetCallbackAllocator(), &fragmentModule);
	}
}