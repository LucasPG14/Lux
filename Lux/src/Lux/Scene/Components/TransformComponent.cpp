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
	
	void TransformComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::Text("Position");
			ImGui::SameLine();
			ImGui::DragFloat3("##Position", glm::value_ptr(position));

			ImGui::Text("Rotation");
			ImGui::SameLine();
			ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation));

			ImGui::Text("Scale");
			ImGui::SameLine();
			ImGui::DragFloat3("##Scale", glm::value_ptr(scale));
		}
	}
}