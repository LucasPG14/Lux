#include "amtpch.h"
#include "MeshComponent.h"

// TODO: This needs to be deleted
#include "Amethyst/Renderer/Renderer.h"

#include <imgui.h>

namespace Amethyst
{
	MeshComponent::MeshComponent(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		vao.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> vbo;
		vbo.reset(VertexBuffer::Create(vertices.data(), sizeof(Vertex) * vertices.size()));

		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT2, "texCoords"},
				//{ShaderDataType::FLOAT3, "normal"}
			};

			vbo->SetLayout(layout);
		}

		std::shared_ptr<IndexBuffer> ebo;
		ebo.reset(IndexBuffer::Create(indices.data(), indices.size()));
		vao->AddIndexBuffer(ebo);

		vao->AddVertexBuffer(vbo);
	}
	
	MeshComponent::~MeshComponent()
	{
	}
	
	void MeshComponent::Update()
	{
		Renderer::Submit(vao);
	}
	
	void MeshComponent::DrawInspector()
	{
	}
}