#pragma once

#include "Vertex.h"

namespace Lux
{
	class Mesh;

	namespace Importer
	{
		void ImportFBX(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const std::string& path);
		const std::shared_ptr<Mesh>& ImportFBX2(const std::string& path, const std::string& assetsPath);
	}
}