#pragma once

#include "Component.h"
#include "Amethyst/Resources/Mesh.h"

#include <glm/glm.hpp>

namespace Amethyst
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(std::shared_ptr<Mesh> meshPtr);

		~MeshComponent();

		void Update() override;

		void DrawInspector() override;

		const std::shared_ptr<Mesh>& GetMesh() { return mesh; }
	private:
		std::shared_ptr<Mesh> mesh;
	};
}