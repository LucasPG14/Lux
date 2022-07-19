#pragma once

#include "Vertex.h"

namespace Amethyst
{
	namespace Importer
	{
		void ImportFBX(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	}
}