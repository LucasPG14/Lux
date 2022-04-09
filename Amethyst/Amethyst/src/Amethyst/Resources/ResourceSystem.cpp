#include "amtpch.h"

#include "ResourceSystem.h"

#include "Amethyst/Utils/Hash.h"

namespace Amethyst
{
	std::unordered_map<std::string, std::shared_ptr<Resource>> ResourceSystem::resources = {};

	ResourceSystem::~ResourceSystem()
	{
	}

	void ResourceSystem::Delete(std::filesystem::path& path)
	{
		resources.erase(path.string());
	}
	
	void ResourceSystem::ImportResources(std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);

		std::uint32_t type = 0;
		file.read((char*)&type, sizeof(std::uint32_t));

		switch (type)
		{
		case TypeID<Mesh>::id():
		{
			ResourceSystem::Create<Mesh>(path);
			return;
		}
		}
	}
}