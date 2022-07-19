#include "luxpch.h"
#include "Renderer.h"

#include "Lux/Scene/Components/TransformComponent.h"
#include "Lux/Scene/Components/LightComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lux
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
		sceneInfo->viewPos = camera.GetPosition();
	}
	
	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Material>& material, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& model)
	{
		shader->Bind();
		shader->UploadUniformMat4("viewProjection", glm::transpose(sceneInfo->viewProjection));
		shader->UploadUniformFloat3("viewPos", sceneInfo->viewPos);
		shader->UploadUniformMat4("model", model);

		//// Light Info
		//for (int i = 0; i < lights.size(); ++i)
		//{
		//	const LightComponent* light = lights[i].second;
		//	switch (light->GetType())
		//	{
		//	case LightType::DIRECTIONAL:
		//		shader->UploadUniformFloat3("dirLight.direction", lights[i].first->GetRotation());
		//		shader->UploadUniformFloat3("dirLight.ambient", light->GetColor());
		//		shader->UploadUniformFloat3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		//		shader->UploadUniformFloat3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//		break;
		//	case LightType::POINT:
		//		shader->UploadUniformFloat3("pointLight.position", lights[i].first->GetPosition());

		//		shader->UploadUniformFloat("pointLight.constant", 1.0f);
		//		shader->UploadUniformFloat("pointLight.quadratic", 1.0f);
		//		shader->UploadUniformFloat("pointLight.linear", 1.0f);


		//		shader->UploadUniformFloat3("pointLight.ambient", light->GetColor());
		//		shader->UploadUniformFloat3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		//		shader->UploadUniformFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//		break;
		//	}
		//}

		// Material info
		material->Bind();
		
		shader->UploadUniformInt("material.albedoMap", 0);
		shader->UploadUniformInt("material.normalMap", 1);

		shader->UploadUniformFloat3("material.albedoColor", material->GetColor());

		vertexArray->Bind();
		RenderOrder::Draw(vertexArray);
	}

	void Renderer::DrawQuad()
	{
		float quad[] = 
		{ 
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};
		
		std::shared_ptr<VertexArray> vao = VertexArray::Create();
		vao->Bind();

		std::shared_ptr<VertexBuffer> vbo = VertexBuffer::Create(quad, 20 * sizeof(float));
		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT2, "texCoords"},
			};

			vbo->SetLayout(layout);
		}
		vao->AddVertexBuffer(vbo);

		uint32_t indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		std::shared_ptr<IndexBuffer> ebo = IndexBuffer::Create(indices, 6);

		vao->AddIndexBuffer(ebo);

		RenderOrder::Draw(vao);
	}

	void Renderer::ChangeState(bool change)
	{
		RenderOrder::ChangeState(change);
	}
}