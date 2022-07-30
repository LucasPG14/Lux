#include "luxpch.h"

#include "MaterialComponent.h"
#include "Lux/Utils/Hash.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

namespace Lux
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
}