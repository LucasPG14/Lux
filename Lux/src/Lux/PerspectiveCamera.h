#pragma once

#include "Core/Timer.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Lux
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera();
		~PerspectiveCamera();

		bool Update(Timer timer);

		inline const glm::mat4& GetViewMatrix() const { return viewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }

		void SetDimensions(float width, float height);

		inline const glm::vec3& GetPosition() const { return position; }
		inline const glm::vec3& GetRight() const { return glm::normalize(glm::cross(front, up)); }
		inline const glm::vec3& GetUp() const { return up; }
		inline const glm::vec3& GetFront() const { return up; }

		inline const float GetAspectRatio() const { return aspectRatio; }
		
		inline const float GetNearPlane() const { return nearPlane; }
		inline const float GetFarPlane() const { return farPlane; }

		inline const float GetHorizontalFov() const { return hFov; }
		inline const float GetVerticalFov() const { return vFov; }

		glm::vec3 NearPlanePos(glm::vec2& pos);
		glm::vec3 FarPlanePos(glm::vec2& pos);
	
	private:	
		const glm::mat3x4 ComputeViewMatrix();

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

		float aspectRatio;

		glm::vec2 mouseInitialPos;
	};
}