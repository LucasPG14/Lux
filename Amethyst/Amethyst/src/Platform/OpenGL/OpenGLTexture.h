#pragma once

#include "Amethyst/Renderer/Texture.h"

namespace Amethyst
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();

		inline uint32_t GetWidth() const override { return width; }
		inline uint32_t GetHeight() const override { return height; }

		void Bind(uint32_t slot = 0) const override;
	private:
		std::string path;

		uint32_t width;
		uint32_t height;
		uint32_t textureID;
	};
}