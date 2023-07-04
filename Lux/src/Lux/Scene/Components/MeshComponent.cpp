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

		//glm::vec3 min = glm::vec3(std::numeric_limits<float>().max());
		//glm::vec3 max = glm::vec3(std::numeric_limits<float>().min());
		//int numTriangles = indices.size() / 3;
		//for (int i = 0; i < numTriangles; ++i)
		//{
		//	AABB aabb;

		//	glm::vec3 v1 = vertices[indices[(i * 3)]].position;
		//	glm::vec3 v2 = vertices[indices[(i * 3) + 1]].position;
		//	glm::vec3 v3 = vertices[indices[(i * 3) + 2]].position;

		//	glm::vec2 texCoordsV1 = vertices[indices[(i * 3)]].texCoords;
		//	glm::vec2 texCoordsV2 = vertices[indices[(i * 3) + 1]].texCoords;
		//	glm::vec2 texCoordsV3 = vertices[indices[(i * 3) + 2]].texCoords;

		//	min = glm::min(min, v1);
		//	min = glm::min(min, v2);
		//	min = glm::min(min, v3);

		//	max = glm::max(max, v1);
		//	max = glm::max(max, v2);
		//	max = glm::max(max, v3);

		//	// Get the normal of this face
		//	glm::vec3 u = v2 - v1;
		//	glm::vec3 v = v3 - v1;

		//	aabb.min = glm::vec4(min, texCoordsV1.x);
		//	aabb.max = glm::vec4(max, texCoordsV1.y);
		//	aabb.texCoords = glm::vec4(texCoordsV2, texCoordsV3);
		//	aabb.normal = glm::vec4(glm::cross(u, v), 0.0);
		//	//aabb.normal.x = (u.y * v.z) - (u.z * v.y);
		//	//aabb.normal.y = (u.z * v.x) - (u.x * v.z);
		//	//aabb.normal.z = (u.x * v.y) - (u.y * v.x);

		//	globalAABB.min = glm::min(globalAABB.min, aabb.min);
		//	globalAABB.max = glm::max(globalAABB.max, aabb.max);

		//	aabbs.push_back(aabb);
		//}

		int indSize = indices.size() / 3;
		for (int i = 0; i < indSize; ++i)
		{
			indices2.push_back(glm::vec4(indices[i * 3], indices[(i * 3) + 1], indices[(i * 3) + 2], 0.0f));
		}

		for (int i = 0; i < vertices.size(); ++i)
		{
			positions.push_back(glm::vec4(vertices[i].position, vertices[i].texCoords.x));
			normals.push_back(glm::vec4(vertices[i].normals, vertices[i].texCoords.y));
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