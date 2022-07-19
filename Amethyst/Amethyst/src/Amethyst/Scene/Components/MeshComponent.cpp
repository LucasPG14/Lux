#include "amtpch.h"
#include "MeshComponent.h"

#include "Amethyst/Utils/Primitives.h"
#include "Amethyst/Utils/Importer.h"

#include <imgui.h>

namespace Amethyst
{
	MeshComponent::MeshComponent()
	{
		//CreateSphere(vertices, indices);
		Importer::ImportFBX(vertices, indices);
		vao = VertexArray::Create();

		vbo = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex));
		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT3, "normals"},
				{ShaderDataType::FLOAT2, "texCoords"},
				{ShaderDataType::FLOAT3, "tangents"},
				{ShaderDataType::FLOAT3, "bitangents"}
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);

		ebo = IndexBuffer::Create(indices.data(), indices.size());
		vao->AddIndexBuffer(ebo);
	}
	
	MeshComponent::~MeshComponent()
	{
	}
	
	void MeshComponent::Update()
	{
	}
	
	void MeshComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{
		}
	}
}