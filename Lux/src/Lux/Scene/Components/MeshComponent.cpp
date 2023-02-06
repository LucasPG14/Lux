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
		Importer::ImportFBX(vertices, indices, "Assets/Models/Cube.obj");

		int numTriangles = indices.size() / 3;
		for (int i = 0; i < numTriangles; ++i)
		{
			AABB& aabb = aabbs.emplace_back();

			glm::vec3 v1 = vertices[indices[(i * 3)]].position;
			glm::vec3 v2 = vertices[indices[(i * 3) + 1]].position;
			glm::vec3 v3 = vertices[indices[(i * 3) + 2]].position;

			aabb.min = glm::min(aabb.min, v1);
			aabb.min = glm::min(aabb.min, v2);
			aabb.min = glm::min(aabb.min, v3);

			aabb.max = glm::max(aabb.max, v1);
			aabb.max = glm::max(aabb.max, v2);
			aabb.max = glm::max(aabb.max, v3);

			// Get the normal of this face
			glm::vec3 u = v2 - v1;
			glm::vec3 v = v3 - v1;

			aabb.normal.x = (u.y * v.z) - (u.z * v.y);
			aabb.normal.y = (u.z * v.x) - (u.x * v.z);
			aabb.normal.z = (u.x * v.y) - (u.y * v.x);

			globalAABB.min = glm::min(globalAABB.min, aabb.min);
			globalAABB.max = glm::max(globalAABB.max, aabb.max);
		}

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