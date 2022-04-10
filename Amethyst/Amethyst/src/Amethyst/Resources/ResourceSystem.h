#pragma once

#include "Resource.h"

// Resources
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace Amethyst
{
	class ResourceSystem
	{
	public:
		virtual ~ResourceSystem();

		template<typename T>
		static std::shared_ptr<T> Create(std::filesystem::path& path)
		{
			return std::static_pointer_cast<T>(resources[path.string()] = std::make_shared<T>(path));
		}

		static void Delete(std::filesystem::path& path);

		// Needs to check if the resource exists
		template<typename T>
		static std::shared_ptr<T> Get(std::filesystem::path& path)
		{
			return std::static_pointer_cast<T>(resources[path.string()]);
		}

		static void ImportResources(std::filesystem::path& path);

	private:
		static std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
	};
}