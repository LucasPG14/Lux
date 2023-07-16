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

		static GLenum GetFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::INT:
				return GL_INT;
			case TextureFormat::FLOAT:
				return GL_FLOAT;
			}
		}

		static GLenum GetInternalFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::INT:
				return GL_RGBA32I;
			case TextureFormat::FLOAT:
				return GL_RGBA32F;
			}
		}
	}

	// Texture 2D
	Texture2D::Texture2D(void* d, int w, int h) : width(w), height(h), data(d)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, d);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(int w, int h) : width(w), height(h)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	Texture2D::Texture2D(const void* data, int w, const TextureSpecification& spec) : width(w), height(1)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, Utilities::GetInternalFormat(spec.format), w, 1, 0, GL_RGBA, Utilities::GetFormat(spec.format), data);
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

	void Texture2D::BindImage(uint32_t slot) const
	{
		glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	void Texture2D::UnbindImage(uint32_t slot) const
	{
		glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}
	
	void Texture2D::Resize(int w, int h)
	{
		width = w;
		height = h;

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}
	
	void Texture2D::SaveToFile()
	{
		//GLuint* data = new GLuint[width * height * 4 * 4];

		//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureID);

		//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);

		////if (stbi_write_png("hola.png", spec.width, spec.height, 4, data, 0))
		//{

		//}

		//glBindTexture(GL_TEXTURE_2D, 0);
	}
}