#include "amtpch.h"

#include "Material.h"
#include "ResourceSystem.h"

namespace Amethyst
{
	Material::Material(UUID id, const std::filesystem::path& filePath)
	{
		loaded = false;
		path = filePath;
		uuid = id;
	}

	Material::~Material()
	{

	}

	void Material::Load()
	{
		if (!loaded)
		{
			std::ifstream file(path, std::ios::binary);

			// Reading the UUID
			file.read((char*)&uuid, sizeof(uint64_t));
			
			// Read all the material information
			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));

			// Reading the UUID
			uint64_t diffUUID = 0;
			file.read((char*)&diffUUID, sizeof(uint64_t));
			
			diffuse = ResourceSystem::Get<OpenGLTexture2D>(diffUUID);
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