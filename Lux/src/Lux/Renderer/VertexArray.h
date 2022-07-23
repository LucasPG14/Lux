#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Lux
{
	class VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex);
		void AddIndexBuffer(const std::shared_ptr<IndexBuffer>& index);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return vertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return indexBuffer; }

	private:
		uint32_t vertexArrayID;
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};
}