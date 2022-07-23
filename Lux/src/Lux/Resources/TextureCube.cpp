#include "luxpch.h"
#include "TextureCube.h"

#include <stb_image.h>
#include <glad/glad.h>

namespace Lux
{
	TextureCube::TextureCube(const std::vector<std::string>& paths)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, channels;
		for (int i = 0; i < paths.size(); ++i)
		{
			stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			stbi_image_free(data);
		}
		stbi_set_flip_vertically_on_load(true);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	TextureCube::~TextureCube()
	{
		glDeleteTextures(1, &textureID);
	}

	void TextureCube::Bind(uint32_t slot) const
	{
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

	void TextureCube::Unbind(uint32_t slot) const
	{
		glDepthFunc(GL_LESS);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}