#include "luxpch.h"

#include "MaterialComponent.h"
#include "Lux/Utils/Hash.h"

#include "Lux/Resources/ResourceManager.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

namespace Lux
{
	MaterialComponent::MaterialComponent() 
	{
		material = ResourceManager::CreateMaterial();
	}
	
	MaterialComponent::~MaterialComponent()
	{
		ResourceManager::DestroyMaterial(material);
	}
	
	void MaterialComponent::Update()
	{
	}
}