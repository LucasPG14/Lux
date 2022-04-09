#pragma once

// TODO: Remove this when importing works correctly
#include "Amethyst/Utils/Vertex.h"
#include "Amethyst/Scene/Components/MeshComponent.h"

namespace Amethyst
{
	namespace Importer
	{
		void Import(std::filesystem::path& path, std::filesystem::path directory);
		MeshComponent* Load(std::filesystem::path& path);
	}
}