#pragma once

#include "Amethyst/Core/UUID.h"

namespace Amethyst
{
	enum class ResourceType
	{
		NONE = 0,
		MESH = 1,
		MATERIAL = 2
	};

	class Resource
	{
	public:
		virtual ~Resource() {}

		virtual void Load() = 0;

		virtual void UnLoad() = 0;

		const std::string GetPath() { return path.string(); }
		const UUID GetUUID() { return uuid; }
		
	protected:
		bool loaded;
		std::filesystem::path path;
		UUID uuid;
	//	std::filesystem::path originalPath;
	};
}