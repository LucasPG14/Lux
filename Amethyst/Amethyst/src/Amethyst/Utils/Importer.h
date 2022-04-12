#pragma once

namespace Amethyst
{
	namespace Importer
	{
		void Import(const std::filesystem::path& path, const std::filesystem::path& directory);
		void ImportTexture(const std::filesystem::path& path, const std::filesystem::path& directory);
	}
}