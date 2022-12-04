#pragma once

#include "Ray.h"
#include <glm/glm.hpp>

namespace Lux
{
	class Camera
	{
	public:
		Camera(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up, float vFov, float aspectRatio,
			float aperture, float focusDist)
		{
			float theta = glm::radians(vFov);
			float h = glm::tan(theta / 2);

			float viewHeight = 2.0f * h;
			float viewWidth = aspectRatio * viewHeight;

			float focalLength = 1.0f;

			w = glm::normalize(lookFrom - lookAt);
			u = glm::normalize(glm::cross(up, w));
			v = glm::cross(w, u);

			origin = lookFrom;
			horizontal = focusDist * viewWidth * u;
			vertical = focusDist * viewHeight * v;
			lowerLeftCorner = origin - (horizontal / 2.0f) - (vertical / 2.0f) - (focusDist * w);

			lensRadius = aperture / 2.0f;
		}

		Ray GetRay(float s, float t)
		{
			glm::vec3 rd = lensRadius * RandomUnitDisk();
			glm::vec3 offset = (u * rd.x) + (v * rd.y);

			return Ray(origin + offset, lowerLeftCorner + (s * horizontal) + (t * vertical) - origin - offset);
		}

	private:
		glm::vec3 origin;
		glm::vec3 lowerLeftCorner;
		glm::vec3 horizontal;
		glm::vec3 vertical;

		glm::vec3 w, u, v;
		float lensRadius;
	};
}