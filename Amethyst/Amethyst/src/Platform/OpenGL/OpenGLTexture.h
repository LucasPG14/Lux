#pragma once

#include "Amethyst/Resources/Texture.h"

namespace Amethyst
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const void* data, int width, int height);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();

		inline uint32_t GetWidth() const override { return width; }
		inline uint32_t GetHeight() const override { return height; }

		inline uint32_t GetID() const { return textureID; }

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const override;

	private:
		uint32_t width;
		uint32_t height;
		uint32_t textureID;
	};
}