#include "amtpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include "glad/glad.h"

namespace Amethyst
{
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
		stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
		AMT_CORE_ASSERT(data, "Couldn't load the image!");
		width = w;
		height = h;

		GLenum internalFormat = 0;
		GLenum dataFormat = 0;

		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		AMT_CORE_ASSERT(internalFormat && dataFormat, "Texture format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTextureStorage2D(textureID, 1, internalFormat, width, height);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(UUID id, const std::filesystem::path& texPath)
	{
		loaded = false;
		path = texPath;
		uuid = id;
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &textureID);
	}

	void OpenGLTexture2D::Load()
	{
		if (!loaded)
		{
			// Opening the file and getting the path
			std::ifstream file(path, std::ios::binary);

			// Reading the UUID
			file.read((char*)&uuid, sizeof(uint64_t));

			// Reading the type of the resource
			std::uint32_t type = 0;
			file.read((char*)&type, sizeof(std::uint32_t));
			
			// Getting the information of width, height and channels of the image
			int channels = 0;
			file.read((char*)&width, sizeof(int));
			file.read((char*)&height, sizeof(int));
			file.read((char*)&channels, sizeof(int));
			
			// Setting the buffer size to get all the image data
			int bufferSize = width * channels * height;
			stbi_uc* data = new stbi_uc[bufferSize];
			file.read((char*)data, bufferSize);
			
			AMT_CORE_ASSERT(data, "Couldn't load the image!");

			GLenum internalFormat = 0;
			GLenum dataFormat = 0;

			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			AMT_CORE_ASSERT(internalFormat && dataFormat, "Texture format not supported!");

			// Creating the image for OpenGL, TODO: Needs must be rewriten to support other APIs
			glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTextureStorage2D(textureID, 1, internalFormat, width, height);

			glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void OpenGLTexture2D::UnLoad()
	{
		loaded = false;
	}
	
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	
	void OpenGLTexture2D::Unbind(uint32_t slot) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}