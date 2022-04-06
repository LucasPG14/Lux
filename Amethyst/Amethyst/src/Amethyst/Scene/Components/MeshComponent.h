#pragma once

#include "Component.h"
#include "Amethyst/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Amethyst
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices);

		~MeshComponent();

	private:
		std::unique_ptr<VertexArray> vao;
	};
}