#include "amtpch.h"
#include "LightComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
{
	LightComponent::LightComponent(LightType t) : color(1.0f, 1.0f, 1.0f), type(t)
	{
	}
	
	LightComponent::~LightComponent()
	{
	}

	void LightComponent::Update()
	{
	}
	
	void LightComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Light Component"))
		{
			ImGui::ColorPicker3("Light Color", glm::value_ptr(color));
		}
	}
}