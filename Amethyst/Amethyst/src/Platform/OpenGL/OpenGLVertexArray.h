#pragma once

#include "Amethyst/Renderer/VertexArray.h"

namespace Amethyst
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex) override;
		void AddIndexBuffer(const std::shared_ptr<IndexBuffer>& index) override;

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return vertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return indexBuffer; }
	private:
		uint32_t vertexArrayID;
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};
}