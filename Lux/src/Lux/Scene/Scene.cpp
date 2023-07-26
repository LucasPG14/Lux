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
		DrawCube({ 1.0, 0.0, 0.0 }, { -1.5f, 0.0f, 0.0f }, { 0.1f, 3.0f, 3.0f });
		DrawCube({ 0.0, 1.0, 0.0 }, { 1.5f, 0.0f, 0.0f }, { 0.1f, 3.0f, 3.0f });
		//DrawCube({ 1.0, 1.0, 1.0 }, { 0.0f, 0.0f, 1.5f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 90.0f, 0.0f});
		DrawCube({ 1.0, 1.0, 1.0 }, { 0.0f, 0.0f, -1.5f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 90.0f, 0.0f});
		DrawCube({ 1.0, 1.0, 1.0 }, { 0.0f, -1.5f, 0.0f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 0.0f, 90.0f});
		DrawCube({ 1.0, 1.0, 1.0 }, { 0.0f, 1.5f, 0.0f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 0.0f, 90.0f});
		
		DrawCube({ 0.9, 0.0, 0.9 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, 0.0f, 0.0f});

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

	void Scene::CollectInformation()
	{
		transforms.clear();
		positions.clear();
		indices.clear();
		normals.clear();
		objectsInfo.clear();

		float transformsCount = 0.0f;

		for (int i = 0; i < world.size(); ++i)
		{
			Entity& entity = world[i];
			if (MeshComponent* comp = entity.Get<MeshComponent>())
			{
				transforms.push_back(entity.Get<TransformComponent>()->GetTransform());
				
				float offsetVertices = positions.size();
				positions.resize(offsetVertices + comp->GetPositions().size());
				std::copy(comp->GetPositions().begin(), comp->GetPositions().end(), &positions[offsetVertices]);
				
				float offsetIndices = indices.size();
				indices.resize(offsetIndices + comp->GetIndices().size());
				std::copy(comp->GetIndices().begin(), comp->GetIndices().end(), &indices[offsetIndices]);

				float offsetNormals = normals.size();
				normals.resize(/*offset + (comp->GetAABBGeometry().size() * sizeof(AABB))*/offsetNormals + comp->GetNormals().size());
				std::copy(comp->GetNormals().begin(), comp->GetNormals().end(), &normals[offsetNormals]);

				ObjectInfo info;
				glm::vec3 color = entity.Get<MaterialComponent>()->GetMaterial()->GetColor();
				info.info.x = color.x;
				info.info.y = color.y;
				info.info.z = color.z;
				info.info.w = transforms.size() - 1;
				objectsInfo.push_back(info);
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
	
	void Scene::DrawCube(const glm::vec3& color, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rot)
	{
		Entity& ent1 = CreateEntity("Cube" + std::to_string(world.size()));
		ent1.CreateComponent<MeshComponent>();
		ent1.CreateComponent<MaterialComponent>();

		ent1.Get<MaterialComponent>()->GetMaterial()->GetColor() = color;

		TransformComponent* transform = ent1.Get<TransformComponent>();
		transform->SetPosition(pos);
		transform->SetScale(scale);
		transform->SetRotation(rot);
	}
}