#include "amtpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"

#include "Amethyst/Renderer/Renderer.h"

namespace Amethyst
{
	Scene::Scene()
	{
	}
	
	Scene::~Scene()
	{

	}
	
	void Scene::Update(const std::shared_ptr<Shader>& shader)
	{
		for (int i = 0; i < world.size(); ++i)
		{
			Entity& entity = world[i];
			if (MeshComponent* mesh = entity.Get<MeshComponent>())
			{
				// TODO: Fix why doesn't render the mesh with a transform component
				Renderer::Submit(shader, mesh->GetVAO(), entity.Get<MaterialComponent>()->GetMaterial()/*, entity.Get<TransformComponent>()->GetTransform()*/);
			}
		}
	}
	
	Entity& Scene::CreateEntity(const std::string& name)
	{
		return world.emplace_back(name);
	}
}