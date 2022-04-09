#pragma once

// TODO: Remove this when importing works correctly
#include "Amethyst/Utils/Vertex.h"

namespace Amethyst
{
	namespace Importer
	{
		void Import(std::filesystem::path& path, std::filesystem::path directory, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	}
}