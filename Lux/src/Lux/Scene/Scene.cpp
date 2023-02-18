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

		for (int i = 0; i < 2; ++i)
		{
			Entity& ent1 = CreateEntity("Cube" + std::to_string(i + 1));
			ent1.CreateComponent<MeshComponent>();
			ent1.CreateComponent<MaterialComponent>();
		}

		shader = CreateSharedPtr<Shader>("Assets/Shaders/Deferred.glsl");

		Entity& dirLight = CreateEntity("Directional Light");
		TransformComponent* transform = dirLight.Get<TransformComponent>();
		transform->SetPosition({ 0.0f, 3.0f, 0.0f });
		transform->SetRotation({ 50.0f, -30.0f, -70.0f });
		AddLight(transform, dirLight.CreateComponent<LightComponent>(LightType::DIRECTIONAL));

		Entity& pointLight = CreateEntity("Spot Light");
		AddLight(pointLight.Get<TransformComponent>(), pointLight.CreateComponent<LightComponent>(LightType::SPOT));
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