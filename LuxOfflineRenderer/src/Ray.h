#pragma once

#include <glm/glm.hpp>

namespace Lux
{
	class Ray
	{
	public:
		Ray(const glm::vec3& ori, const glm::vec3& dir) : origin(ori), direction(dir)
		{

		}

		const glm::vec3& GetOrigin() { return origin; }
		const glm::vec3& GetDirection() { return direction; }

		const glm::vec3 at(float t) const
		{
			return origin + (t * direction);
		}

	public:
		glm::vec3 origin;
		glm::vec3 direction;
	};
}