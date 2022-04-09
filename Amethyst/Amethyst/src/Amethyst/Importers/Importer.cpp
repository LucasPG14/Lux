#include "amtpch.h"
#include "Importer.h"

#include "Amethyst/Utils/Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Amethyst
{
	namespace Importer
	{
		void Import(std::filesystem::path& path, std::filesystem::path directory, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
		{
			Assimp::Importer importer;

			// Reading the file
			const aiScene* scene = importer.ReadFile(path.string(), aiProcessPreset_TargetRealtime_MaxQuality);

			// Check if the file has been readed correctly
			if (scene)
			{
				for (int i = 1; i < scene->mNumMeshes; ++i)
				{
					aiMesh* mesh = scene->mMeshes[i];

					/*std::vector<Vertex> vertices;
					std::vector<uint32_t> indices;*/

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

					//std::filesystem::path pathToSave = directory / path.filename().replace_extension("");
					//pathToSave += "_" + meshName;
					//pathToSave.replace_extension("bsres");
					//
					//std::ofstream saveFile(pathToSave.string(), std::ios::binary);

					//saveFile << numVertices;
					//saveFile << numIndices;
					//saveFile << vertices.data();
					//saveFile << indices.data();

					//saveFile.close();
				}
			}
		}
	}
}