#pragma once

#include <glm/glm.hpp>

namespace Lux
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texCoords = { 0.0f, 0.0f };
		glm::vec3 tangents = { 0.0f, 0.0f, 0.0f };
		glm::vec3 bitangents = { 0.0f, 0.0f, 0.0f };
	};

	struct AABB
	{
		glm::vec4 min;
		glm::vec4 max;
		glm::vec4 texCoords;
		glm::vec4 normal;
		//glm::vec3 normal;
	};
}