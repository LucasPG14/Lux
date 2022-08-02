#include "luxpch.h"
#include "Renderer.h"

#include "VertexArray.h"
#include "VertexBuffer.h"

#include "Lux/Scene/Components/TransformComponent.h"
#include "Lux/Scene/Components/LightComponent.h"

#include "Lux/Utils/Tools.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

namespace Lux
{
	std::unique_ptr<Renderer::SceneInfo> Renderer::sceneInfo = std::make_unique<SceneInfo>();
	std::unique_ptr<ShaderLibrary> Renderer::shaderLibrary = std::make_unique<ShaderLibrary>();

	std::shared_ptr<VertexArray> Renderer::quadVao = nullptr;
	std::shared_ptr<VertexBuffer> Renderer::quadVbo = nullptr;
	std::shared_ptr<IndexBuffer> Renderer::quadEbo = nullptr;
	
	
	void Renderer::Init()
	{
		// Setting rendering specification
		glEnable(GL_TEXTURE_2D);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		std::shared_ptr<Shader> shader = CreateSharedPtr<Shader>("Assets/Shaders/Texture.glsl");
		shaderLibrary->Add(shader);

		float quad[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		quadVao = CreateSharedPtr<VertexArray>();
		quadVao->Bind();

		quadVbo = CreateSharedPtr<VertexBuffer>(quad, 20 * sizeof(float));
		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT2, "texCoords"},
			};

			quadVbo->SetLayout(layout);
		}
		quadVao->AddVertexBuffer(quadVbo);

		uint32_t indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		quadEbo = CreateSharedPtr<IndexBuffer>(indices, 6);

		quadVao->AddIndexBuffer(quadEbo);
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

		// Setting view and projection matrix from the camera and the model matrix of the object
		shader->SetUniformMat4("viewProjection", glm::transpose(sceneInfo->viewProjection));
		shader->SetUniformFloat3("viewPos", sceneInfo->viewPos);
		shader->SetUniformMat4("model", model);

		// Material bindings and uniforms
		material->Bind();
		shader->SetUniformInt("material.diffuseMap", 0);
		shader->SetUniformInt("material.normalMap", 1);
		shader->SetUniformInt("material.metallicMap", 2);
		shader->SetUniformInt("material.roughnessMap", 3);
		shader->SetUniformFloat3("material.albedoColor", material->GetColor());

		vertexArray->Bind();
		Draw(vertexArray);
	}

	void Renderer::DrawFullscreenQuad()
	{
		quadVao->Bind();
		Draw(quadVao);
	}

	void Renderer::DrawSkybox(const std::shared_ptr<VertexArray>& vao, const std::shared_ptr<TextureCube>& cubemap, const std::shared_ptr<Shader>& shader, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
	{
		shader->Bind();

		shader->SetUniformMat4("projection", projMatrix);
		shader->SetUniformMat4("view", glm::mat3(viewMatrix));

		cubemap->Bind();
		shader->SetUniformInt("skybox", 0);

		vao->Bind();
		Draw(vao);
		cubemap->Unbind();
	}

	void Renderer::ChangeState(bool change)
	{
		if (change) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);
	}

	void Renderer::ClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void Renderer::Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
}