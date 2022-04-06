#include "amtpch.h"
#include "TransformComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
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
	
	void TransformComponent::DrawInspector()
	{
		ImGui::DragFloat3("Position", glm::value_ptr(position));
	}
}