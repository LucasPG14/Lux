#include "amtpch.h"
#include "Importer.h"

#include "Amethyst/Utils/Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

namespace Amethyst
{
	namespace Importer
	{
		void Import(std::filesystem::path& path, std::filesystem::path directory)
		{
			Assimp::Importer importer;

			// Reading the file
			const aiScene* scene = importer.ReadFile(path.string(), aiProcessPreset_TargetRealtime_MaxQuality);

			// Check if the file has been readed correctly
			if (scene)
			{
				for (int i = 0; i < scene->mNumMeshes; ++i)
				{
					aiMesh* mesh = scene->mMeshes[i];

					std::vector<Vertex> vertices;
					std::vector<uint32_t> indices;

					std::string meshName = mesh->mName.C_Str();
					int numVertices = mesh->mNumVertices;
					int numIndices = mesh->mNumFaces * 3;

					vertices.reserve(numVertices);
					indices.reserve(numIndices);

					// Reading all the vertices
					for (int j = 0; j < numVertices; ++j)
					{
						Vertex& vertex = vertices.emplace_back();

						vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y , mesh->mVertices[j].z };

						if (mesh->HasTextureCoords(0))
							vertex.texCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
					}

					for (int j = 0; j < mesh->mNumFaces; ++j)
					{
						aiFace face = mesh->mFaces[j];

						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}
		
					std::filesystem::path pathToSave = directory / path.filename().replace_extension("");
					pathToSave += "_" + meshName;
					pathToSave.replace_extension("bsres");
					
					std::ofstream saveFile(pathToSave.string(), std::ios::binary);

					// Storing all information first to create the mesh correctly
					saveFile.write((char*)&numVertices, sizeof(int));
					saveFile.write((char*)&numIndices, sizeof(int));
					saveFile.write((char*)vertices.data(), sizeof(Vertex) * vertices.size());
					saveFile.write((char*)indices.data(), sizeof(uint32_t) * indices.size());
					//saveFile << numIndices;
					//saveFile << vertices.data();
					//saveFile << indices.data();

					saveFile.close();
				}

				for (int i = 0; i < scene->mNumMaterials; ++i)
				{
					aiMaterial* material = scene->mMaterials[i];

					int index = material->GetTextureCount(aiTextureType_DIFFUSE);

					aiString str;
					if (material->GetTexture(aiTextureType_DIFFUSE, index, &str))
					{
						int width = 0, height = 0;
						int channels = 0;

						stbi_uc* data = stbi_load(str.C_Str(), &width, &height, &channels, 0);

						if (data)
						{
							bool ret = true;
							ret = false;
						}
					}
				}
			}
		}
		
		MeshComponent* Load(std::filesystem::path& path)
		{
			std::ifstream file(path.c_str(), std::ios::binary);

			int numVertices = 0;
			int numIndices = 0;

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			file.read((char*)&numVertices, sizeof(int));
			file.read((char*)&numIndices, sizeof(int));
			
			vertices.resize(numVertices);
			indices.resize(numIndices);
			
			file.read((char*)vertices.data(), sizeof(Vertex) * numVertices);
			file.read((char*)indices.data(), sizeof(uint32_t) * numIndices);

			MeshComponent* mesh = new MeshComponent(vertices, indices);
			return mesh;
		}
	}
}