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

	// This should not be here, needs to be removed
	void Mesh::Update()
	{
		Renderer::Submit(vao);
	}
	
	void Mesh::Load()
	{
		if (!loaded)
		{
			// Reading the file
			std::ifstream file(path, std::ios::binary);

			// TODO: Read the mesh information and create the vao
			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));

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