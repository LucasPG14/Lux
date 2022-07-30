#include "luxpch.h"
#include "MeshComponent.h"

#include "Lux/Utils/Primitives.h"
#include "Lux/Utils/Importer.h"

#include <imgui.h>

namespace Lux
{
	MeshComponent::MeshComponent()
	{
		//CreateSphere(vertices, indices);
		Importer::ImportFBX(vertices, indices, "Assets/Models/model.fbx");
		vao = CreateSharedPtr<VertexArray>();

		vbo = CreateSharedPtr<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
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

		ebo = CreateSharedPtr<IndexBuffer>(indices.data(), indices.size());
		vao->AddIndexBuffer(ebo);
	}
	
	MeshComponent::~MeshComponent()
	{
	}
	
	void MeshComponent::Update()
	{
	}
}