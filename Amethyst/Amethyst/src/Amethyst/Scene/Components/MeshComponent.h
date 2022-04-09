#pragma once

#include "Component.h"
#include "Amethyst/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Amethyst
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

		~MeshComponent();

		void Update() override;

		void DrawInspector() override;

	private:
		std::shared_ptr<VertexArray> vao;
	};
}