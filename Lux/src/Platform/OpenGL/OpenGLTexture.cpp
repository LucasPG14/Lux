#include "luxpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include "glad/glad.h"

namespace Lux
{
	namespace Utilities
	{
		static void GetGLFormat(int channels, GLenum& internalFormat, GLenum& dataFormat)
		{
			switch (channels)
			{
			case 4:
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case 3:
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			}

			LUX_CORE_ASSERT(internalFormat && dataFormat, "Texture format not supported!");
		}
	}

	// Texture 2D
	OpenGLTexture2D::OpenGLTexture2D(const void* data, int w, int h) : width(w), height(h)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		// TODO: Check this for HDR images
		//if (stbi_is_hdr(path.c_str()))
		//{
		//	bool ret = true;
		//	ret = false;
		//}
		stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
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

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &textureID);
	}
	
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	
	void OpenGLTexture2D::Unbind(uint32_t slot) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Texture Cube

	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& paths)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, channels;
		for (int i = 0; i < paths.size(); ++i)
		{
			stbi_set_flip_vertically_on_load(1);
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

	void OpenGLTextureCube::Unbind(uint32_t slot) const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}