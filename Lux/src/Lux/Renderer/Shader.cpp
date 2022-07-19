#include "luxpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Lux
{
	std::shared_ptr<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE:
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLShader>(filePath);
		}
		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertex, const std::string& fragment)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE: 
		{
			LUX_CORE_ASSERT(false, "There's no RendererAPI");
			return nullptr;
		}
		case Render::API::OPENGL: return std::make_shared<OpenGLShader>(name, vertex, fragment);
		}
		LUX_CORE_ASSERT(false, "RendererAPI not defined!");
		return nullptr;
	}
	
	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		const std::string& name = shader->GetName();

		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		LUX_CORE_ASSERT(shaders.find(name) == shaders.end(), "This shader already exists!")
		shaders[name] = shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		std::shared_ptr<Shader> shader = Shader::Create(filepath);
		Add(shader);

		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		std::shared_ptr<Shader> shader = Shader::Create(filepath);
		Add(name, shader);

		return shader;
	}

	const std::shared_ptr<Shader>& ShaderLibrary::Get(const std::string& name)
	{
		LUX_CORE_ASSERT(shaders.find(name) != shaders.end(), "This shader doesn't exists!");
		return shaders[name];
	}

}