#include "amtpch.h"

#include "Mesh.h"

#include "Amethyst/Renderer/Renderer.h"

namespace Amethyst
{
	Mesh::Mesh(UUID id, const std::filesystem::path& filePath)
	{
		loaded = false;
		path = filePath;
		uuid = id;
	}

	Mesh::~Mesh()
	{

	}
	
	void Mesh::Load()
	{
		if (!loaded)
		{
			// Reading the file
			std::ifstream file(path, std::ios::binary);

			// Mesh UUID
			file.read((char*)&uuid, sizeof(uint64_t));

			// Type of the resource
			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));

			// Reading the material path
			uint64_t matUUID = 0;
			file.read((char*)&matUUID, sizeof(uint64_t));

			// Reading the information of the geometry
			int numVertices = 0;
			int numIndices = 0;
			file.read((char*)&numVertices, sizeof(int));
			file.read((char*)&numIndices, sizeof(int));

			//std::vector<Vertex> vertices;
			//std::vector<uint32_t> indices;

			vertices.resize(numVertices);
			indices.resize(numIndices);

			file.read((char*)vertices.data(), sizeof(Vertex) * numVertices);
			file.read((char*)indices.data(), sizeof(uint32_t) * numIndices);

			file.read((char*)&aabb, sizeof(AABB));

			size_t strSize = 0;
			file.read((char*)&strSize, sizeof(size_t));

			std::string shaderName;
			shaderName.resize(strSize);
			file.read(shaderName.data(), strSize);

			meshShader = Renderer::GetShaderLibrary()->Get(shaderName);

			// Setting the vertex array with all the information
			vao = VertexArray::Create();

			std::shared_ptr<VertexBuffer> vbo;
			vbo = VertexBuffer::Create(vertices.data(), sizeof(Vertex) * vertices.size());

			{
				BufferLayout layout =
				{
					{ShaderDataType::FLOAT3, "position"},
					{ShaderDataType::FLOAT3, "normal"},
					{ShaderDataType::FLOAT2, "texCoords"},
					{ShaderDataType::FLOAT3, "tangents"},
					{ShaderDataType::FLOAT3, "bitangents"}
				};

				vbo->SetLayout(layout);
			}

			std::shared_ptr<IndexBuffer> ebo;
			ebo = IndexBuffer::Create(indices.data(), indices.size());
			vao->AddIndexBuffer(ebo);

			vao->AddVertexBuffer(vbo);

			loaded = true;
		}
	}
	
	void Mesh::UnLoad()
	{
		loaded = false;
	}
}