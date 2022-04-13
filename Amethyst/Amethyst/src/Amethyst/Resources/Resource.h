#pragma once

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
		
	protected:
		bool loaded;
		std::filesystem::path path;
	//	std::filesystem::path originalPath;
	};
}