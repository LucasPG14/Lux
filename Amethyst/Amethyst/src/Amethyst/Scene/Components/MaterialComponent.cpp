#include "amtpch.h"

#include "MaterialComponent.h"
#include "Amethyst/Utils/Hash.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

namespace Amethyst
{
	MaterialComponent::MaterialComponent() 
	{
		material = std::make_shared<Material>();
	}
	
	MaterialComponent::~MaterialComponent()
	{
	}
	
	void MaterialComponent::Update()
	{
	}
	
	void MaterialComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Material Component"))
		{
			ImGui::ColorPicker3("##color", glm::value_ptr(material->GetColor()));
		}
	}
}