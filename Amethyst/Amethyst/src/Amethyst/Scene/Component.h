#pragma once

#include <glm/glm.hpp>

namespace Amethyst
{
	enum class ComponentType
	{
		TRANSFORM = 0,
		MESH, 
		MATERIAL
	};

	struct Component
	{
		
	};

	struct Transform : public Component
	{
		glm::mat4 transform;
	};
}