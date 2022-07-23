#pragma once

namespace Lux
{
	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const { return count; }

	private:
		uint32_t indexID;
		uint32_t count;
	};
}