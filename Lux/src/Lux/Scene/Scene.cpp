#include "luxpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightComponent.h"

#include "Lux/Renderer/Renderer.h"

namespace Lux
{
	Scene::Scene()
	{
		//Entity& entity = CreateEntity("Main Camera");
		//entity.CreateComponent<CameraComponent>();

		Entity& ent = CreateEntity("Sphere");
		ent.CreateComponent<MeshComponent>();
		ent.CreateComponent<MaterialComponent>();

		shader = CreateSharedPtr<Shader>("Assets/Shaders/Deferred.glsl");

		Entity& dirLight = CreateEntity("Directional Light");
		AddLight(dirLight.Get<TransformComponent>(), dirLight.CreateComponent<LightComponent>(LightType::DIRECTIONAL));

		Entity& pointLight = CreateEntity("Point Light");
		AddLight(pointLight.Get<TransformComponent>(), pointLight.CreateComponent<LightComponent>(LightType::POINT));
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
				Renderer::Submit(shader, entity.Get<MaterialComponent>()->GetMaterial(), mesh->GetVertexArray(), entity.Get<TransformComponent>()->GetTransform());
			}
		}
	}
	
	Entity& Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	
	Entity& Scene::CreateEntityWithUUID(UUID id, const std::string& name)
	{
		return world.emplace_back(id, name);
	}
}