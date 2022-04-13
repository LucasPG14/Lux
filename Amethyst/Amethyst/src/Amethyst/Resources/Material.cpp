#include "amtpch.h"

#include "Material.h"
#include "ResourceSystem.h"

namespace Amethyst
{
	Material::Material(const std::filesystem::path& filePath)
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
			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));

			size_t strSize = 0;
			std::string path;
			file.read((char*)&strSize, sizeof(size_t));
			path.resize(strSize);
			file.read(path.data(), strSize);
			
			diffuse = ResourceSystem::Get<OpenGLTexture2D>(std::filesystem::path(path));
			diffuse->Load();

			loaded = true;
		}
	}

	void Material::UnLoad()
	{
		loaded = false;
	}
	
	void Material::Bind()
	{
		diffuse->Bind();
	}
	
	void Material::Unbind()
	{
		diffuse->Unbind();
	}
}