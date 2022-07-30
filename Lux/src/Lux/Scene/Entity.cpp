#include "luxpch.h"
#include "Entity.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"

#include <imgui.h>

namespace Lux
{
	Entity::Entity(UUID id, const std::string& n) : uuid(id), name(n)
	{
		CreateComponent<TransformComponent>(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}
	
	Entity::~Entity()
	{

	}
	
	void Entity::Update()
	{
	}
}