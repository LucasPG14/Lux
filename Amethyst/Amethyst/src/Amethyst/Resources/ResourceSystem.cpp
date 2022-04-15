#include "amtpch.h"

#include "ResourceSystem.h"

#include "Amethyst/Utils/Hash.h"

namespace Amethyst
{
	std::unordered_map<UUID, std::shared_ptr<Resource>> ResourceSystem::resources = {};

	ResourceSystem::~ResourceSystem()
	{
	}

	void ResourceSystem::Delete(UUID uuid)
	{
		resources.erase(uuid);
	}
	
	void ResourceSystem::ImportResources(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);

		uint64_t uuid = 0;
		file.read((char*)&uuid, sizeof(uint64_t));

		std::uint32_t type = 0;
		file.read((char*)&type, sizeof(std::uint32_t));

		switch (type)
		{
		case TypeID<Mesh>::id():
		{
			ResourceSystem::CreateWithUUID<Mesh>(uuid, path);
			return;
		}
		case TypeID<Material>::id():
		{
			ResourceSystem::CreateWithUUID<Material>(uuid, path);
			return;
		}
		case TypeID<OpenGLTexture2D>::id():
		{
			ResourceSystem::CreateWithUUID<OpenGLTexture2D>(uuid, path);
			return;
		}
		}
	}
}