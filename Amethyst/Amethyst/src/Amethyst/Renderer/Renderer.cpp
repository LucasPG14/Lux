#include "amtpch.h"
#include "Renderer.h"

#include "Amethyst/Resources/ResourceSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
{
	std::unique_ptr<Renderer::SceneInfo> Renderer::sceneInfo = std::make_unique<SceneInfo>();
	std::unique_ptr<ShaderLibrary> Renderer::shaderLibrary = std::make_unique<ShaderLibrary>();

	void Renderer::Init()
	{
		RenderOrder::Init();

		// OpenGL
		std::shared_ptr<Shader> shader = Shader::Create("Assets/Shaders/Texture.glsl");
		shaderLibrary->Add(shader);
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		sceneInfo->viewProjection = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	}
	
	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{
		material->Bind();

		shader->Bind();
		shader->UploadUniformMat4("viewProjection", glm::transpose(sceneInfo->viewProjection));
		shader->UploadUniformMat4("model", model);

		vertexArray->Bind();
		RenderOrder::Draw(vertexArray);
	}

	void Renderer::ChangeState(bool change)
	{
		RenderOrder::ChangeState(change);
	}
}