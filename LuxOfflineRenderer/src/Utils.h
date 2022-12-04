#pragma once

#include <glm/glm.hpp>
#include <glm/glm/gtx/norm.hpp>

namespace Lux
{
	inline float FloatRandom()
	{
		return rand() / (RAND_MAX + 1.0f);
	}

	inline float FloatRandom(float min, float max)
	{
		return min + (max - min) * FloatRandom();
	}

	glm::vec3 RandomUnitSphere()
	{
		while (true)
		{
			glm::vec3 p = { FloatRandom(-1.0f, 1.0f), FloatRandom(-1.0f, 1.0f), FloatRandom(-1.0f, 1.0f) };
			if (glm::length2(p) >= 1)
				continue;

			return p;
		}
	}

	glm::vec3 RandomUnitDisk()
	{
		while (true)
		{
			glm::vec3 p = { FloatRandom(-1.0f, 1.0f), FloatRandom(-1.0f, 1.0f), 0.0f };
			if (glm::length2(p) >= 1)
				continue;

			return p;
		}
	}

	glm::vec3 RandomUnitVector()
	{
		return glm::normalize(RandomUnitSphere());
	}

	glm::vec3 RandomHemisphere(const glm::vec3& normal)
	{
		glm::vec3 unitSphere = RandomUnitSphere();

		if (glm::dot(unitSphere, normal) > 0.0f)
			return unitSphere;
		else
			return -unitSphere;
	}
}