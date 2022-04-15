#pragma once

#include <glm/glm.hpp>

namespace Amethyst
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texCoords;
		//glm::vec3 tangents = { 0.0f, 0.0f, 0.0f };
		//glm::vec3 bitangents = { 0.0f, 0.0f, 0.0f };
	};

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};
}