#pragma once

#include "Component.h"
#include "Amethyst/Utils/Vertex.h"

#include "Amethyst/Renderer/VertexArray.h"
#include "Amethyst/Renderer/Buffer.h"

#include <glm/glm.hpp>

namespace Amethyst
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent();

		~MeshComponent();

		void Update() override;

		void DrawInspector() override;

		const std::shared_ptr<VertexArray>& GetVertexArray() { return vao; }
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;
	};
}