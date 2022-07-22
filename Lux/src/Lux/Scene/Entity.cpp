#include "luxpch.h"
#include "Entity.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightComponent.h"

#include <imgui.h>

namespace Lux
{
	Entity::Entity(UUID id, const std::string& n) : uuid(id), name(n), aabbDebug(false)
	{
		TransformComponent* transform = new TransformComponent({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f });
		components.push_back(transform);
	}
	
	Entity::~Entity()
	{

	}
	
	void Entity::Update()
	{
		for (int i = 0; i < components.size(); ++i)
			components[i]->Update();
	}
	
	void Entity::DrawInspector()
	{
		ImGui::InputText("##name", name.data(), 128);
		ImGui::Checkbox("Draw AABB", &aabbDebug);
		for (int i = 0; i < components.size(); ++i)
		{
			components[i]->DrawInspector();
		}
	}
}