#include "luxpch.h"
#include "LightComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	LightComponent::LightComponent(LightType t) : color(1.0f, 1.0f, 1.0f), lightType(t)
	{
	}
	
	LightComponent::~LightComponent()
	{
	}

	void LightComponent::Update()
	{
	}
}