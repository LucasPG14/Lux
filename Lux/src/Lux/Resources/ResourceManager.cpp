#include "luxpch.h"
#include "ResourceManager.h"

#include "Mesh.h"
#include "Material.h"

namespace Lux
{
	uint32_t ResourceManager::materialsCount = 0;
	std::vector<std::shared_ptr<Material>> ResourceManager::materials = {};
	std::vector<std::shared_ptr<Mesh>> ResourceManager::meshes = {};

	std::shared_ptr<Material> ResourceManager::CreateMaterial()
	{
		std::shared_ptr<Material> material = std::make_shared<Material>(materialsCount++);
		
		materials.push_back(material);
		return material;
	}

	const std::shared_ptr<Mesh>& ResourceManager::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& path, const std::string& origPath)
	{
		std::shared_ptr<Mesh> mesh = CreateSharedPtr<Mesh>(vertices, indices, path, origPath);
		
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