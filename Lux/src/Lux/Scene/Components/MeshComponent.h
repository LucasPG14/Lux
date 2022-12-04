#pragma once

#include "Component.h"
#include "Lux/Utils/Vertex.h"

#include "Lux/Renderer/VertexArray.h"
#include "Lux/Renderer/Buffer.h"

#include <glm/glm.hpp>

namespace Lux
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent();

		~MeshComponent();

		void Update() override;

		const std::shared_ptr<VertexArray>& GetVertexArray() { return vao; }

		const std::vector<AABB>& GetAABB() { return aabbs; }

		ComponentType GetType() { return type; }
		COMPONENT_TYPE(MESH)
	private:
		ComponentType type;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<AABB> aabbs;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;
	};
}