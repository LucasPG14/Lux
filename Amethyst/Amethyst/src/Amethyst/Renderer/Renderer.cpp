#include "amtpch.h"
#include "Renderer.h"

namespace Amethyst
{
	std::unique_ptr<Renderer::SceneInfo> Renderer::sceneInfo = std::make_unique<SceneInfo>();
	std::unique_ptr<ShaderLibrary> Renderer::shaderLibrary = std::make_unique<ShaderLibrary>();

	struct RendererData
	{
		static const uint32_t maxTriangles = 20000;
		static const uint32_t maxVertices = maxTriangles * 4;
		static const uint32_t maxIndices = maxTriangles * 6;
		static const uint32_t maxTextures = 32;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> whiteTexture;

		uint32_t triangleIndexCount = 0;

		std::array<std::shared_ptr<Texture2D>, maxTextures> textures;
		uint32_t textureIndex = 1; // 0 is for the white texture

		Renderer::Statistics stats;
	};

	static RendererData data;

	void Renderer::Init()
	{
		RenderOrder::Init();

		// Loading all shaders
		std::shared_ptr<Shader> shader = Shader::Create("assets/shaders/Texture.glsl");
		shaderLibrary->Add(shader);

		data.vao = VertexArray::Create();
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		sceneInfo->viewProjection = camera.GetViewMatrix() * camera.GetProjectionMatrix();

		StartBatch();
	}
	
	void Renderer::EndScene()
	{
		Flush();
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{
		material->Bind();

		shader->Bind();
		shader->UploadUniformMat4("viewProjection", sceneInfo->viewProjection);
		shader->UploadUniformMat4("model", model);

		vertexArray->Bind();
		RenderOrder::Draw(vertexArray);
	}
	
	void Renderer::StartBatch()
	{
		data.triangleIndexCount = 0;

		data.textureIndex = 1;
	}
	
	void Renderer::Flush()
	{
		if (data.triangleIndexCount == 0)
			return;



		RenderOrder::Draw(data.vao);
		data.stats.drawCalls++;
	}
	
	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}
}