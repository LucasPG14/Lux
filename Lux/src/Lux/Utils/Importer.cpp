#include "luxpch.h"
#include "Importer.h"

#include "Hash.h"
#include "Lux/Resources/ResourceManager.h"
#include "Lux/Resources/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

namespace Lux
{
	namespace Importer
	{
		void ImportFBX(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const std::string& path)
		{
			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

			if (scene == nullptr)
			{
				LUX_CORE_ERROR("Couldn't find the file");
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
					if (mesh->HasTextureCoords(0))
					{
						vertex.texCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
					}
					if (mesh->HasTangentsAndBitangents())
					{
						vertex.tangents = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
						vertex.bitangents = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
					}
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

		std::shared_ptr<Mesh> ImportFBX2(const std::string& path, const std::string& assetsPath)
		{
			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

			if (scene == nullptr)
			{
				LUX_CORE_ERROR("Couldn't find the file");
				return nullptr;
			}

			for (int i = 0; i < scene->mNumMeshes; ++i)
			{
				AABB aabb;
				glm::vec3 min = glm::vec3(std::numeric_limits<float>().max());
				glm::vec3 max = glm::vec3(std::numeric_limits<float>().min());
				aiMesh* mesh = scene->mMeshes[i];

				std::vector<Vertex> vertices;

				vertices.reserve(mesh->mNumVertices);

				for (int j = 0; j < mesh->mNumVertices; ++j)
				{
					Vertex& vertex = vertices.emplace_back();

					vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
					vertex.normals = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
					if (mesh->HasTextureCoords(0))
					{
						vertex.texCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
					}
					if (mesh->HasTangentsAndBitangents())
					{
						vertex.tangents = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
						vertex.bitangents = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
					}

					min = glm::min(min, vertex.position);
					max = glm::max(max, vertex.position);
				}
				aabb.min = glm::vec4(min, 0.0);
				aabb.max = glm::vec4(max, 0.0);

				std::vector<uint32_t> indices;
				indices.reserve(mesh->mNumFaces * 3);
				for (int j = 0; j < mesh->mNumFaces; ++j)
				{
					const aiFace& face = mesh->mFaces[j];

					for (int k = 0; k < face.mNumIndices; ++k)
						indices.push_back(face.mIndices[k]);
				}

				std::filesystem::path resourcePath = "Library/";
				resourcePath += mesh->mName.C_Str();
				resourcePath += ".rmesh";

				return ResourceManager::CreateMesh(aabb, vertices, indices, resourcePath.string(), path);
			}
		}
	}
}