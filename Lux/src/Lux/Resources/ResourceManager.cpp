#include "luxpch.h"
#include "ResourceManager.h"

#include "Mesh.h"
#include "Material.h"

namespace Lux
{
	uint32_t ResourceManager::materialsCount = 0;
	std::vector<std::shared_ptr<Material>> ResourceManager::materials = {};
	std::vector<std::shared_ptr<Mesh>> ResourceManager::meshes = {};

	void ResourceManager::LoadFile(const std::filesystem::path& filepath)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		
		unsigned int* header = new unsigned int[3];
		std::ifstream file;
		file.open(filepath.c_str(), std::ios::out | std::ios::app | std::ios::binary);
	}

	void ResourceManager::DestroyMaterial(const std::shared_ptr<Material>& material)
	{
		for (auto it = materials.begin(); it < materials.end(); ++it)
		{
			if (*it == material)
			{
				materials.erase(it);
				break;
			}
		}
	}

	std::shared_ptr<Material> ResourceManager::CreateMaterial()
	{
		std::shared_ptr<Material> material = std::make_shared<Material>(materialsCount++);
		
		materials.push_back(material);
		return material;
	}

	const std::shared_ptr<Mesh>& ResourceManager::CreateMesh(const AABB& aabb, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& path, const std::string& origPath)
	{
		std::shared_ptr<Mesh> mesh = CreateSharedPtr<Mesh>(aabb, vertices, indices, path, origPath);
		
		meshes.push_back(mesh);
		return mesh;
	}
	
	const std::shared_ptr<Mesh>& ResourceManager::GetMesh(const std::string& path)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			if (meshes[i]->GetOriginalPath() == path)
				return meshes[i];
		}

		return nullptr;
	}
}