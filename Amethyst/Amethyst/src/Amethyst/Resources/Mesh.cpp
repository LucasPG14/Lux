#include "amtpch.h"

#include "Mesh.h"

#include "Amethyst/Renderer/Renderer.h"

namespace Amethyst
{
	Mesh::Mesh(std::filesystem::path& filePath)
	{
		loaded = false;
		path = filePath;
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

			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));

			// Reading the material path
			size_t matSize = 0;
			std::string material;
			file.read((char*)&matSize, sizeof(size_t));
			material.resize(matSize);
			file.read(material.data(), matSize);

			// Reading the information of the geometry
			int numVertices = 0;
			int numIndices = 0;
			file.read((char*)&numVertices, sizeof(int));
			file.read((char*)&numIndices, sizeof(int));

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			vertices.resize(numVertices);
			indices.resize(numIndices);

			file.read((char*)vertices.data(), sizeof(Vertex) * numVertices);
			file.read((char*)indices.data(), sizeof(uint32_t) * numIndices);

			// Setting the vertex array with all the information
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

			loaded = true;
		}
	}
	
	void Mesh::UnLoad()
	{
		loaded = false;
	}
}