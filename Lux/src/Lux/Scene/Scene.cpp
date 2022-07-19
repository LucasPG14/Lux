#include "luxpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightComponent.h"

#include "Lux/Renderer/Renderer.h"

// TODO: Remove this
#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	Scene::Scene()
	{
		//Entity& entity = CreateEntity("Main Camera");
		//entity.CreateComponent<CameraComponent>();

		Entity& ent = CreateEntity("Sphere");
		MeshComponent* mesh = new MeshComponent();
		ent.AddComponent(mesh);

		MaterialComponent* material = new MaterialComponent();
		ent.AddComponent(material);
		//CreateEntity("Light");
		shader = Shader::Create("Assets/Shaders/Deferred.glsl");

		Entity& light = CreateEntity("Directional Light");
		AddLight(light.Get<TransformComponent>(), light.CreateComponent<LightComponent>(LightType::DIRECTIONAL));
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