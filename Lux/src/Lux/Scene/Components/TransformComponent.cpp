#include "luxpch.h"
#include "TransformComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	TransformComponent::TransformComponent(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) 
		: position(pos), rotation(rot), scale(sca)
	{	
	}
	
	TransformComponent::~TransformComponent()
	{
	}
	
	void TransformComponent::Update()
	{
	}
}