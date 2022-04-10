#include "amtpch.h"

#include "MaterialComponent.h"

namespace Amethyst
{
	MaterialComponent::MaterialComponent(std::shared_ptr<Material> mat) : material(mat)
	{
		material->Load();
	}
	
	MaterialComponent::~MaterialComponent()
	{
	}
	
	void MaterialComponent::Update()
	{
	}
	
	void MaterialComponent::DrawInspector()
	{
	}
}