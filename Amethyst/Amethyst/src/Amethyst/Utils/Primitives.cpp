#include "amtpch.h"
#include "Primitives.h"

#include "Amethyst/Resources/ResourceSystem.h"
#include "Amethyst/Utils/Hash.h"

#include "Vertex.h"

#include <stb_image.h>

namespace Amethyst
{
	#define M_PI    3.14159265358979323846264338327950288

	void CreateSphere()
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		constexpr AABB aabb = { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} };

		float z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / 1.0f;    // vertex normal
		float s, t;                                     // vertex texCoord

		constexpr float sectorStep = 2 * M_PI / 20;
		constexpr float stackStep = M_PI / 20;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= 20; ++i)
		{
			stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = 1.0f * cosf(stackAngle);             // r * cos(u)
			z = 1.0f * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= 20; ++j)
			{
				Vertex& vertex = vertices.emplace_back();
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				vertex.position.x = xy * cosf(sectorAngle);
				vertex.position.y = xy * sinf(sectorAngle);
				vertex.position.z = z;

				// normalized vertex normal (nx, ny, nz)
				vertex.normals.x = vertex.position.x * lengthInv;
				vertex.normals.y = vertex.position.y * lengthInv;
				vertex.normals.z = vertex.position.z * lengthInv;

				// vertex tex coord (s, t) range between [0, 1]
				vertex.texCoords.x = (float)j / 20;
				vertex.texCoords.y = (float)i / 20;
			}
		}

		int k1, k2;
		for (int i = 0; i < 20; ++i)
		{
			k1 = i * (20 + 1);
			k2 = k1 + 20 + 1;

			for (int j = 0; j < 20; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.emplace_back(k1);
					indices.emplace_back(k2);
					indices.emplace_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (20 - 1))
				{
					indices.emplace_back(k1 + 1);
					indices.emplace_back(k2);
					indices.emplace_back(k2 + 1);
				}
			}
		}

		// Creating the material path and file
		std::filesystem::path matPath("Editor/Materials/Default.bsres");
		std::ofstream matFile(matPath, std::ios::binary);

		// Creating the Material resource
		uint64_t materialUUID = ResourceSystem::Create<Material>(matPath);
		matFile.write((char*)&materialUUID, sizeof(uint64_t));

		constexpr std::uint32_t type = TypeID<Material>::id();
		matFile.write((char*)&type, sizeof(std::uint32_t));

		std::filesystem::path texturePath("Editor/Textures/WhiteTexture.png");

		// Using STB to load the image
		int width, height, channels;
		stbi_uc* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 0);

		if (data)
		{
			// TODO: Now it's is used the OpenGLTexture but this needs to be changed
			// because the engine will support more Render APIs 

			// Setting the path for the texture
			texturePath = "Editor/Textures/WhiteTexture.bsres";

			// Creating the file to save the image
			std::ofstream texFile(texturePath, std::ios::binary);

			uint64_t texUUID = ResourceSystem::Create<OpenGLTexture2D>(texturePath);

			constexpr std::uint32_t texType = TypeID<OpenGLTexture2D>::id();

			texFile.write((char*)&texUUID, sizeof(uint64_t));
			texFile.write((char*)&texType, sizeof(std::uint32_t));
			texFile.write((char*)&width, sizeof(int));
			texFile.write((char*)&height, sizeof(int));
			texFile.write((char*)&channels, sizeof(int));

			int buffSize = width * channels * height;

			//int bufferSize = width * height;
			texFile.write((char*)data, buffSize);

			texFile.close(); // Important to close the file when finished

			matFile.write((char*)&texUUID, sizeof(uint64_t));
		}

		matFile.close();

		int numVertices = vertices.size();
		int numIndices = indices.size();

		// Storing the sphere
		std::filesystem::path pathToSave = "editor/Meshes/Sphere.bsres";

		std::ofstream meshFile(pathToSave, std::ios::binary);

		// Storing the UUID
		uint64_t uuid = ResourceSystem::Create<Mesh>(pathToSave);
		meshFile.write((char*)&uuid, sizeof(uint64_t));

		// Storing the hash of Mesh to get the type when loading
		constexpr std::uint32_t meshType = TypeID<Mesh>::id();
		meshFile.write((char*)&meshType, sizeof(std::uint32_t));

		// Storing the material UUID attached to this mesh
		meshFile.write((char*)&materialUUID, sizeof(uint64_t));

		// Storing the geometry information
		meshFile.write((char*)&numVertices, sizeof(int));
		meshFile.write((char*)&numIndices, sizeof(int));
		meshFile.write((char*)vertices.data(), sizeof(Vertex) * vertices.size());
		meshFile.write((char*)indices.data(), sizeof(uint32_t) * indices.size());

		meshFile.write((char*)&aabb, sizeof(AABB));

		std::string shaderName = "Texture";

		size_t strSize = shaderName.size();
		meshFile.write((char*)&strSize, sizeof(size_t));
		meshFile.write(shaderName.data(), strSize);

		meshFile.close();
	}
}