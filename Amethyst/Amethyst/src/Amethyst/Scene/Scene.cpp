#include "amtpch.h"
#include "Scene.h"

namespace Amethyst
{
	Scene::Scene()
	{
		entities.emplace_back("Entity1");
		entities.emplace_back("Entity2");
		entities.emplace_back("Entity3");
	}
	
	Scene::~Scene()
	{

	}
	
	void Scene::Update()
	{

	}
}