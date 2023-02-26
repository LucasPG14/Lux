#pragma once

namespace Lux
{
	class Texture2D
	{
	public:
		Texture2D(void* data, int width, int height);
		Texture2D(int width, int height);
		Texture2D(const void* data, int w);
		Texture2D(const std::string& path);
		Texture2D();
		virtual ~Texture2D();

		uint32_t GetWidth() const { return width; }
		uint32_t GetHeight() const { return height; }

		uint32_t GetID() const { return textureID; }

		void Bind(uint32_t slot = 0) const;
		void Unbind(uint32_t slot = 0) const;

		void BindImage(uint32_t slot = 0) const;
		void UnbindImage(uint32_t slot = 0) const;

		void Resize(int w, int h);

		void AddData(const void* data, int s);

		const void* GetData() { return data; }

	private:
		uint32_t width;
		uint32_t height;
		uint32_t textureID;

		uint32_t size;
		uint32_t offset;

		void* data;
	};
}