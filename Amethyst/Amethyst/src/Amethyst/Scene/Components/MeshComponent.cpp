#include "amtpch.h"
#include "MeshComponent.h"

#include <imgui.h>

namespace Amethyst
{
	MeshComponent::MeshComponent(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
	{
		//std::shared_ptr<VertexBuffer> vbo;
		//vbo.reset(VertexBuffer::Create(vertices.data(), sizeof(vertices)));

		//{
		//	BufferLayout layout =
		//	{
		//		{ShaderDataType::FLOAT3, "position"},
		//		{ShaderDataType::FLOAT4, "color"},
		//		//{ShaderDataType::FLOAT3, "normal"}
		//	};

		//	vbo->SetLayout(layout);
		//}

		//vao->AddVertexBuffer(vbo);
	}
	
	MeshComponent::~MeshComponent()
	{
	}
}