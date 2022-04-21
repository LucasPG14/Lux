#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace Amethyst
{
	class CameraComponent : public Component
	{
	public:
		CameraComponent();

		~CameraComponent();

		void Update() override;

		void DrawInspector() override;

		const glm::mat4& GetViewMatrix() { return viewMatrix; }
		const glm::mat4& GetProjectionMatrix() { return projectionMatrix; }

	private:
		float nearPlane;
		float farPlane;

		float hFov;
		float vFov;

		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
}