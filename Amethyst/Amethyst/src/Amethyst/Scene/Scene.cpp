#include "amtpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/MeshComponent.h"

#include "Amethyst/Renderer/Renderer.h"

namespace Amethyst
{
	Scene::Scene()
	{
	}
	
	Scene::~Scene()
	{

	}
	
	void Scene::Update()
	{
		for (int i = 0; i < world.size(); ++i)
		{
			Entity& entity = world[i];
			if (MeshComponent* mesh = entity.Get<MeshComponent>())
			{
				Renderer::Submit(mesh->GetVAO());
			}
		}
	}
	
	Entity& Scene::CreateEntity(const std::string& name)
	{
		return world.emplace_back(name);
	}
}