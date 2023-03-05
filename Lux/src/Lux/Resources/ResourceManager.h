#pragma once

namespace Lux
{
	class Material;

	class ResourceManager
	{
	public:

		static std::shared_ptr<Material> CreateMaterial();

	private:
		static std::vector<std::shared_ptr<Material>> materials;
		static uint32_t materialsCount;
	};
}