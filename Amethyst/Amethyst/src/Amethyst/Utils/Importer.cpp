#include "amtpch.h"
#include "Importer.h"

#include "Hash.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

namespace Amethyst
{
	namespace Importer
	{
		void ImportFBX(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
		{
			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile("Assets/Models/model.fbx", aiProcessPreset_TargetRealtime_MaxQuality);

			if (scene == nullptr)
			{
				AMT_CORE_ERROR("Couldn't find the file");
				return;
			}

			for (int i = 0; i < scene->mNumMeshes; ++i)
			{
				aiMesh* mesh = scene->mMeshes[i];

				vertices.reserve(mesh->mNumVertices);

				for (int j = 0; j < mesh->mNumVertices; ++j)
				{
					Vertex& vertex = vertices.emplace_back();

					vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
					vertex.normals = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
					vertex.texCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
					vertex.tangents = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
					vertex.bitangents = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
				}

				indices.reserve(mesh->mNumFaces * 3);
				for (int j = 0; j < mesh->mNumFaces; ++j)
				{
					const aiFace& face = mesh->mFaces[j];

					for (int k = 0; k < face.mNumIndices; ++k)
						indices.push_back(face.mIndices[k]);
				}
			}
		}
	}
}