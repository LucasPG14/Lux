#include "amtpch.h"

#include "Material.h"

namespace Amethyst
{
	Material::Material(std::filesystem::path& filePath)
	{
		loaded = false;
		path = filePath;
	}

	Material::~Material()
	{

	}

	void Material::Load()
	{
		if (!loaded)
		{
			std::ifstream file(path, std::ios::binary);

			// Read all the material information

			loaded = true;
		}
	}

	void Material::UnLoad()
	{
		loaded = false;
	}
}