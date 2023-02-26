#include "luxpch.h"
#include "Texture2DArray.h"

#include "Lux/Resources/Material.h"
#include "Lux/Resources/Texture2D.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Lux
{
	Texture2DArray::Texture2DArray(const std::shared_ptr<Material>& material)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const void* data = material->GetDiffuse()->GetData();
		if (data)
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, material->GetDiffuse()->GetWidth(), material->GetDiffuse()->GetHeight(), 1900, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	Texture2DArray::Texture2DArray(const std::string& filename)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(1);

		int w, h, channels;
		uint8_t* data = stbi_load(filename.c_str(), &w, &h, &channels, 4);
		
		if (data)
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, w, h, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	Texture2DArray::~Texture2DArray()
	{
	}
	
	void Texture2DArray::AddMaterial(const std::shared_ptr<Material>& material)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		const void* data = material->GetDiffuse()->GetData();
		if (data)
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, material->GetDiffuse()->GetWidth(), material->GetDiffuse()->GetHeight(), 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	void Texture2DArray::AddTexture(const std::string& filename)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

		stbi_set_flip_vertically_on_load(1);

		int w, h, channels;
		uint8_t* data = stbi_load(filename.c_str(), &w, &h, &channels, 4);

		if (data)
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 2048, 2048, GL_RGBA8, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	
	void Texture2DArray::Bind(uint32_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	}
}