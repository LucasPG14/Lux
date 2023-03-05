#include "luxpch.h"
#include "ResourceManager.h"

#include "Material.h"

namespace Lux
{
	uint32_t ResourceManager::materialsCount = 0;
	std::vector<std::shared_ptr<Material>> ResourceManager::materials = {};

	std::shared_ptr<Material> ResourceManager::CreateMaterial()
	{
		std::shared_ptr<Material> material = std::make_shared<Material>(materialsCount++);
		
		materials.push_back(material);
		return material;
	}
}