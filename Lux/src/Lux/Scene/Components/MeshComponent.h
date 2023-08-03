#pragma once

#include "Component.h"
#include "Lux/Utils/Vertex.h"

#include "Lux/Renderer/VertexArray.h"
#include "Lux/Renderer/Buffer.h"

#include <glm/glm.hpp>

namespace Lux
{
	class Mesh;

	class MeshComponent : public Component
	{
	public:
		MeshComponent(const std::string& path);

		~MeshComponent();

		void Update() override;

		const std::shared_ptr<Mesh>& GetMesh() { return mesh; }

		const std::shared_ptr<VertexArray>& GetVertexArray() { return vao; }

		std::vector<AABB>& GetAABBGeometry() { return aabbs; }
		const AABB& GetAABB() { return globalAABB; }

		const std::vector<glm::vec4>& GetPositions();
		const std::vector<glm::vec4>& GetIndices();
		const std::vector<glm::vec4>& GetNormals();

		ComponentType GetType() { return type; }
		COMPONENT_TYPE(MESH)
	private:
		ComponentType type;

		std::shared_ptr<Mesh> mesh;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<AABB> aabbs;
		AABB globalAABB;

		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> normals;
		std::vector<glm::vec4> indices2;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;
	};
}