#include "amtpch.h"
#include "Scene.h"

#include "Components/Component.h"

namespace Amethyst
{
	Scene::Scene()
	{
		Entity entity = world.emplace_back("Entity1");
		entity = world.emplace_back("Entity2");
		entity = world.emplace_back("Entity3");
		entity = world.emplace_back("Entity4");
	}
	
	Scene::~Scene()
	{

	}
	
	void Scene::Update()
	{

	}
	
	Entity& Scene::CreateEntity()
	{
		return world.emplace_back("Baker_House");
	}
}