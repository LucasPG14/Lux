#include "luxpch.h"
#include "Texture2D.h"

#include <stb_image.h>
#include <glad/glad.h>

namespace Lux
{
	namespace Utilities
	{
		static void GetGLFormat(int channels, GLenum& internalFormat, GLenum& dataFormat)
		{
			switch (channels)
			{
			case 1:
			{
				internalFormat = GL_R8;
				dataFormat = GL_RED;
				break;
			}
			case 3:
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case 4:
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			}

			LUX_CORE_ASSERT(internalFormat && dataFormat, "Texture format not supported!");
		}
	}

	// Texture 2D
	Texture2D::Texture2D(const void* data, int w, int h) : width(w), height(h)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(const std::string& path)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);

		uint8_t* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
		LUX_CORE_ASSERT(data, "Couldn't load the image!");
		width = w;
		height = h;

		GLenum internalFormat = 0;
		GLenum dataFormat = 0;

		Utilities::GetGLFormat(channels, internalFormat, dataFormat);

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTextureStorage2D(textureID, 1, internalFormat, width, height);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &textureID);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void Texture2D::Unbind(uint32_t slot) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}