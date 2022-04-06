#pragma once

#include "Component.h"
#include <glm/glm.hpp>

namespace Amethyst
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca);

		~TransformComponent();

		void Update() override;

		void DrawInspector() override;

	private:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}