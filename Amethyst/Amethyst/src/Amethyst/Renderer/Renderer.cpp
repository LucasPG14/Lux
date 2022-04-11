#include "amtpch.h"
#include "Renderer.h"

namespace Amethyst
{
	std::unique_ptr<Renderer::SceneInfo> Renderer::sceneInfo = std::make_unique<SceneInfo>();

	void Renderer::Init()
	{
		RenderOrder::Init();
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		sceneInfo->view = camera.GetViewMatrix();
		sceneInfo->projection = camera.GetProjectionMatrix();
	}
	
	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{
		material->Bind();

		shader->Bind();
		shader->UploadUniformMat4("view", sceneInfo->view);
		shader->UploadUniformMat4("projection", sceneInfo->projection);
		shader->UploadUniformMat4("model", model);

		vertexArray->Bind();
		RenderOrder::Draw(vertexArray);

		material->Unbind();
	}
}