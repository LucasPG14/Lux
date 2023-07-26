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

		void ImportFBX2(const std::string& path, const std::string& assetsPath)
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
				}

				std::vector<uint32_t> indices;
				indices.reserve(mesh->mNumFaces * 3);
				for (int j = 0; j < mesh->mNumFaces; ++j)
				{
					const aiFace& face = mesh->mFaces[j];

					for (int k = 0; k < face.mNumIndices; ++k)
						indices.push_back(face.mIndices[k]);
				}

				std::filesystem::path resourcePath = assetsPath;
				resourcePath += mesh->mName.C_Str();
				resourcePath += ".rmesh";

				std::shared_ptr<Mesh> resource = ResourceManager::CreateMesh(vertices, indices, resourcePath.string(), path);

				std::ofstream file;
				file.open(resourcePath.c_str(), std::ios::out | std::ios::app | std::ios::binary);

				unsigned int header[3] = { vertices.size(), indices.size(), path.size() };

				uint32_t size = sizeof(header) + sizeof(Vertex) * vertices.size() + sizeof(uint32_t) * indices.size() + sizeof(char) * path.size();

				char* buffer = new char[size];
				char* cursor = buffer;

				uint32_t bytes = sizeof(header);
				memcpy(cursor, header, bytes);
				cursor += bytes;

				bytes = sizeof(Vertex) * vertices.size();
				memcpy(cursor, vertices.data(), bytes);
				cursor += bytes;

				bytes = sizeof(unsigned int) * indices.size();
				memcpy(cursor, indices.data(), bytes);
				cursor += bytes;

				bytes = sizeof(char) * path.size();
				memcpy(cursor, path.data(), bytes);
				cursor += bytes;

				file.write(buffer, size);
				file.close();
			}
		}
	}
}