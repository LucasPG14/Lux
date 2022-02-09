#pragma once

#include "Core/Timer.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Amethyst
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera();
		~PerspectiveCamera();

		bool Update(Timer timer);

		inline glm::mat4 GetViewMatrix() const { return viewMatrix; }
		inline glm::mat4 GetProjectionMatrix() const { return projectionMatrix; }

		void SetDimensions(float width, float height);
	private:
		glm::mat3x4 ComputeViewMatrix();
	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;

		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;

		float hFov;
		float vFov;

		float nearPlane;
		float farPlane;

		glm::vec2 mouseInitialPos;
	};
}