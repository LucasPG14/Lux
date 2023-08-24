#include "luxpch.h"
#include "Mesh.h"

#include "Lux/Renderer/VertexArray.h"
#include "Lux/Renderer/Buffer.h"
#include "Lux/Core/UUID.h"

namespace Lux
{
	Mesh::Mesh(const AABB& ab, const std::vector<Vertex>& vert, const std::vector<uint32_t>& ind, const std::string& p, const std::string& origPath)
		: vertices(vert), indices(ind), path(p), originalPath(origPath), aabb(ab)
	{
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

		for (int i = 0; i < vert.size(); ++i)
		{
			positions.push_back(glm::vec4(vertices[i].position, vertices[i].texCoords.x));
			normals.push_back(glm::vec4(vertices[i].normals, vertices[i].texCoords.y));
		}

		int indSize = indices.size() / 3;
		for (int i = 0; i < indSize; ++i)
		{
			indicesRT.push_back(glm::vec4(indices[i * 3], indices[(i * 3) + 1], indices[(i * 3) + 2], 0.0f));
		}
	}

	Mesh::~Mesh()
	{
	}
}