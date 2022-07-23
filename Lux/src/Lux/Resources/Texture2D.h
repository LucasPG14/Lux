#pragma once

namespace Lux
{
	class Texture2D
	{
	public:
		Texture2D(const void* data, int width, int height);
		Texture2D(const std::string& path);
		virtual ~Texture2D();

		uint32_t GetWidth() const { return width; }
		uint32_t GetHeight() const { return height; }

		uint32_t GetID() const { return textureID; }

		void Bind(uint32_t slot = 0) const;
		void Unbind(uint32_t slot = 0) const;

	private:
		uint32_t width;
		uint32_t height;
		uint32_t textureID;
	};
}