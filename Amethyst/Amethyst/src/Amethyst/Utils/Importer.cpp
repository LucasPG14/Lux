#include "amtpch.h"
#include "Importer.h"

#include "Amethyst/Utils/Vertex.h"

#include "Amethyst/Resources/ResourceSystem.h"

#include "Amethyst/Utils/Hash.h"

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
				for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
				{
					aiMesh* mesh = scene->mMeshes[i];

					std::vector<Vertex> vertices;
					std::vector<uint32_t> indices;

					std::string meshName = mesh->mName.C_Str();
					unsigned int numVertices = mesh->mNumVertices;
					unsigned int numIndices = mesh->mNumFaces * 3;

					vertices.reserve(numVertices);
					indices.reserve(numIndices);

					// Reading all the vertices
					for (unsigned int j = 0; j < numVertices; ++j)
					{
						Vertex& vertex = vertices.emplace_back();

						vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y , mesh->mVertices[j].z };

						if (mesh->HasTextureCoords(0))
							vertex.texCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
					}

					for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
					{
						aiFace face = mesh->mFaces[j];

						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}
		
					std::filesystem::path pathToSave = directory / path.filename().replace_extension("");
					pathToSave += "_" + meshName;
					pathToSave.replace_extension("bsres");
					
					std::ofstream meshFile(pathToSave.string(), std::ios::binary);

					// Storing the hash of Mesh to get the type when loading
					constexpr std::uint32_t type = TypeID<Mesh>::id();
					meshFile.write((char*)&type, sizeof(std::uint32_t));

					// Storing the material path attached to this mesh
					aiString nameMat;
					scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME, nameMat);
					std::filesystem::path materialPath = directory / nameMat.C_Str();
					materialPath += ".bsres"; // This is the own format of the engine

					size_t pathSize = materialPath.string().size();
					meshFile.write((char*)&pathSize, sizeof(size_t));
					meshFile.write(materialPath.string().data(), pathSize);

					// Storing the geometry information
					meshFile.write((char*)&numVertices, sizeof(int));
					meshFile.write((char*)&numIndices, sizeof(int));
					meshFile.write((char*)vertices.data(), sizeof(Vertex) * vertices.size());
					meshFile.write((char*)indices.data(), sizeof(uint32_t) * indices.size());

					meshFile.close();

					ResourceSystem::Create<Mesh>(pathToSave);
				}

				for (int i = 0; i < scene->mNumMaterials; ++i)
				{
					aiMaterial* material = scene->mMaterials[i];

					// Getting the name of the material
					aiString matName; 
					material->Get(AI_MATKEY_NAME, matName);

					// Creating the material path and file
					std::filesystem::path matPath(directory / matName.C_Str());
					std::ofstream matFile(matPath.replace_extension(".bsres"), std::ios::binary);

					constexpr std::uint32_t type = TypeID<Material>::id();
					matFile.write((char*)&type, sizeof(std::uint32_t));

					// Getting Diffuse Type first
					aiString assimpTexPath;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &assimpTexPath);

					std::filesystem::path texturePath(assimpTexPath.C_Str());

					if (!texturePath.has_parent_path())
					{
						texturePath = std::filesystem::path(path).remove_filename() / texturePath.filename();
					}

					// Using STB to load the image
					int width, height, channels;
					stbi_uc* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 0);

					if (data)
					{
						// TODO: Now it's is used the OpenGLTexture but this needs to be changed
						// because the engine will support more Render APIs 

						// Setting the path for the texture
						texturePath = directory / texturePath.filename().replace_extension("bsres");

						// Creating the file to save the image
						std::ofstream texFile(texturePath, std::ios::binary);

						constexpr std::uint32_t type = TypeID<OpenGLTexture2D>::id();

						texFile.write((char*)&type, sizeof(std::uint32_t));
						texFile.write((char*)&width, sizeof(int));
						texFile.write((char*)&height, sizeof(int));
						texFile.write((char*)&channels, sizeof(int));

						int buffSize = width * channels * height;

						//int bufferSize = width * height;
						texFile.write((char*)data, buffSize);

						texFile.close(); // Important to close the file when finished

						// Saving this texture in the material
						size_t texSize = texturePath.string().size();

						matFile.write((char*)&texSize, sizeof(size_t));
						matFile.write(texturePath.string().data(), texSize);

						// Finally creating the Texture resource
						ResourceSystem::Create<OpenGLTexture2D>(texturePath);
					}

					stbi_image_free(data);

					matFile.close();

					// Creating the Material resource
					ResourceSystem::Create<Material>(matPath);
				}

				// Check for embedded textures
				for (unsigned int i = 0; i < scene->mNumTextures; ++i)
				{
					aiTexture* texture = scene->mTextures[i];

					int width = texture->mWidth;
					int height = texture->mHeight;
				}
			}
		}
		
		//MeshComponent* Load(std::filesystem::path& path)
		//{
		//	std::ifstream file(path.c_str(), std::ios::binary);

		//	int numVertices = 0;
		//	int numIndices = 0;

		//	std::vector<Vertex> vertices;
		//	std::vector<uint32_t> indices;

		//	file.read((char*)&numVertices, sizeof(int));
		//	file.read((char*)&numIndices, sizeof(int));
		//	
		//	vertices.resize(numVertices);
		//	indices.resize(numIndices);
		//	
		//	file.read((char*)vertices.data(), sizeof(Vertex) * numVertices);
		//	file.read((char*)indices.data(), sizeof(uint32_t) * numIndices);

		//	//MeshComponent* mesh = new MeshComponent(vertices, indices);
		//	return mesh;
		//}
	}
}