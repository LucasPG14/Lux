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
		glm::vec3 min = glm::vec3(std::numeric_limits<float>().max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>().min());
		glm::vec3 normal;
	};
}