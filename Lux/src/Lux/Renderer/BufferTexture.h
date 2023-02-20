#pragma once

namespace Lux
{
	class BufferTexture
	{
	public:
		BufferTexture(void* data, uint32_t size);
		~BufferTexture();

		void AddData(void* data, uint32_t size);

	private:
		uint32_t buffer;
		uint32_t texture;

		int actualSize;
	};
}