#pragma once

#include "Resource.h"

// Resources
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include "Amethyst/Core/UUID.h"

namespace Amethyst
{
	class ResourceSystem
	{
	public:
		virtual ~ResourceSystem();

		// Needs to check if the resource exists
		template<typename T>
		static UUID Create(const std::filesystem::path& path)
		{
			return CreateWithUUID<T>(UUID(), path);
		}

		template<typename T>
		static UUID CreateWithUUID(UUID uuid, const std::filesystem::path& path)
		{
			std::static_pointer_cast<T>(resources[uuid] = std::make_shared<T>(uuid, path));
			return uuid;
		}

		static void Delete(UUID uuid);

		// Needs to check if the resource exists
		template<typename T>
		static std::shared_ptr<T> Get(UUID uuid)
		{
			return std::static_pointer_cast<T>(resources[uuid]);
		}

		static void ImportResources(const std::filesystem::path& path);

		inline static bool Has(UUID uuid)
		{
			return resources.find(uuid) != resources.end();
		}

	private:
		static std::unordered_map<UUID, std::shared_ptr<Resource>> resources;
	};
}