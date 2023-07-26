#pragma once

#include "Vertex.h"

namespace Lux
{
	namespace Importer
	{
		void ImportFBX(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const std::string& path);
		void ImportFBX2(const std::string& path, const std::string& assetsPath);
	}
}