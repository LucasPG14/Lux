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
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::DragFloat3("##Position", glm::value_ptr(position));
			ImGui::SameLine();
			ImGui::Text("Position");

			ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation));
			ImGui::SameLine();
			ImGui::Text("Rotation");

			ImGui::DragFloat3("##Scale", glm::value_ptr(scale));
			ImGui::SameLine();
			ImGui::Text("Scale");
		}
	}
}