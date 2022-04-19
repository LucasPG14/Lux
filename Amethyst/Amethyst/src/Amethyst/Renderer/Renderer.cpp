#include "amtpch.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
{
	std::unique_ptr<Renderer::SceneInfo> Renderer::sceneInfo = std::make_unique<SceneInfo>();
	std::unique_ptr<ShaderLibrary> Renderer::shaderLibrary = std::make_unique<ShaderLibrary>();

	struct Triangle
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texCoords;
		glm::vec3 tangents;
		glm::vec3 bitangents;
		int matIndex;
	};

	struct RendererData
	{
		static const uint32_t maxTriangles = 20000;
		static const uint32_t maxVertices = maxTriangles * 4;
		static const uint32_t maxIndices = maxTriangles * 6;
		static const uint32_t maxMaterials = 32;

		Triangle* triangleBufferBase = nullptr;
		Triangle* triangleBufferPtr = nullptr;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Material> whiteMaterial;

		uint32_t triangleIndexCount = 0;

		std::array<std::shared_ptr<Material>, maxMaterials> materials;
		uint32_t materialIndex = 0; // 0 is for the white texture

		Renderer::Statistics stats;
	};

	static RendererData data;

	void Renderer::Init()
	{
		RenderOrder::Init();

		// Loading all shaders
		std::shared_ptr<Shader> shader = Shader::Create("assets/shaders/Texture.glsl");
		data.shader = Shader::Create("assets/shaders/TextureBatch.glsl");
		shaderLibrary->Add(data.shader);
		shaderLibrary->Add(shader);

		data.vao = VertexArray::Create();

		data.vbo = VertexBuffer::Create(data.maxVertices * sizeof(Triangle));
		data.vbo->SetLayout({
			{ ShaderDataType::FLOAT4, "position" },
			{ ShaderDataType::FLOAT3, "normals" },
			{ ShaderDataType::FLOAT2, "texCoords" },
			{ ShaderDataType::FLOAT3, "tangents" },
			{ ShaderDataType::FLOAT3, "bitangents" },
			{ ShaderDataType::INT,    "matIndex" }
			});
		data.vao->AddVertexBuffer(data.vbo);

		data.triangleBufferBase = new Triangle[data.maxVertices];

		uint32_t* quadIndices = new uint32_t[data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < data.maxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			offset += 3;
		}

		std::shared_ptr<IndexBuffer> ebo = IndexBuffer::Create(quadIndices, data.maxIndices);
		data.vao->AddIndexBuffer(ebo);
		delete[] quadIndices;

		uint32_t whiteTextureData = 0xffffffff;
		//data.whiteMaterial = std::make_shared<Material>(&whiteTextureData, 1, 1);
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
		shader->UploadUniformMat4("viewProjection", glm::transpose(sceneInfo->viewProjection));
		shader->UploadUniformMat4("model", model);

		vertexArray->Bind();
		RenderOrder::Draw(vertexArray);
	}

	void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4 model)
	{
		std::vector<Vertex>& vertices = mesh->GetVertices();

		if (data.triangleIndexCount >= RendererData::maxIndices)
			NextBatch();

		float materialIndex = 0.0f;
		for (uint32_t i = 0; i < data.materialIndex; i++)
		{
			if (data.materials[i] == material)
			{
				materialIndex = (float)i;
				break;
			}
		}

		if (materialIndex == 0.0f)
		{
			if (data.materialIndex >= RendererData::maxMaterials)
				NextBatch();

			materialIndex = (float)data.materialIndex;
			data.materials[data.materialIndex] = material;
			data.materialIndex++;
		}

		for (size_t i = 0; i < vertices.size(); i++)
		{
			data.triangleBufferPtr->position = model * glm::vec4(vertices[i].position, 1.0f);
			data.triangleBufferPtr->normals = vertices[i].normals;
			data.triangleBufferPtr->texCoords = vertices[i].texCoords;
			data.triangleBufferPtr->tangents = vertices[i].tangents;
			data.triangleBufferPtr->bitangents = vertices[i].bitangents;
			data.triangleBufferPtr->matIndex = materialIndex;
			data.triangleBufferPtr++;
		}

		data.triangleIndexCount += mesh->GetIndicesSize();

		data.stats.trianglesCount++;
	}

	void Renderer::ChangeState(bool change)
	{
		RenderOrder::ChangeState(change);
	}
	
	void Renderer::StartBatch()
	{
		data.triangleIndexCount = 0;
		data.triangleBufferPtr = data.triangleBufferBase;

		data.materialIndex = 0;
	}
	
	void Renderer::Flush()
	{
		if (data.triangleIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)data.triangleBufferPtr - (uint8_t*)data.triangleBufferBase);
		data.vbo->SetData(data.triangleBufferBase, dataSize);

		// Bind materials
		for (uint32_t i = 0; i < data.materialIndex; i++)
			data.materials[i]->Bind();

		data.shader->Bind();
		data.shader->UploadUniformMat4("viewProjection", sceneInfo->viewProjection);
		RenderOrder::Draw(data.vao, data.triangleIndexCount);
		data.stats.drawCalls++;
	}
	
	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}
}