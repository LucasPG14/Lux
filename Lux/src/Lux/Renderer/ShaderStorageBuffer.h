#pragma once

namespace Lux
{
	class ShaderStorageBuffer
	{
	public:
		ShaderStorageBuffer(const void* data, uint32_t size, uint32_t binding);
		~ShaderStorageBuffer();

		void Bind();

		uint32_t GetBindingIndex() { return bindingIndex; }

	private:
		uint32_t ssbo;
		uint32_t bindingIndex;
	};
}