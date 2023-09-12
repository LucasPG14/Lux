#include "luxpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightComponent.h"

#include "Lux/Resources/Mesh.h"

#include "Lux/Renderer/Renderer.h"

namespace Lux
{
	Scene::Scene() : change(Change::NONE), numTextures(0)
	{
		Entity* dirLight = CreateEntity("Directional Light");
		TransformComponent* transform = dirLight->Get<TransformComponent>();
		transform->SetPosition({ 0.0f, 3.0f, 0.0f });
		transform->SetRotation({ 50.0f, -30.0f, -70.0f });
		AddLight(transform, dirLight->CreateComponent<LightComponent>(LightType::DIRECTIONAL));
		
		//// Scene ShaderToy
		//DrawCube("Assets/Models/Cube.obj", { 0.12, 0.45, 0.15, 1.0f }, { -1.5f, 0.0f, 0.0f }, { 0.1f, 3.0f, 3.0f });
		//DrawCube("Assets/Models/Cube.obj", { 0.65, 0.05, 0.05, 1.0f }, { 1.5f, 0.0f, 0.0f }, { 0.1f, 3.0f, 3.0f });
		//DrawCube("Assets/Models/Cube.obj", { 0.73, 0.73, 0.73, 1.0f }, { 0.0f, 0.0f, -1.5f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 90.0f, 0.0f});
		//DrawCube("Assets/Models/Cube.obj", { 0.73, 0.73, 0.73, 1.0f }, { 0.0f, -1.5f, 0.0f }, { 0.1f, 3.0f, 3.0f }, {0.0f, 0.0f, 90.0f});
		//DrawCube("Assets/Models/Cube.obj", { 0.73, 0.73, 0.73, 1.0f }, {0.0f, 1.5f, 0.0f}, {0.1f, 3.0f, 3.0f}, {0.0f, 0.0f, 90.0f});
		//
		DrawCube("Assets/Models/Cube.obj", { 0.73, 0.73, 0.73, 1.0f }, { -0.5f, -0.75f, -0.75f }, { 0.75f, 1.5f, 0.75f }, { 0.0f, 14.0f, 0.0f });
		//
		//DrawCube("Assets/Models/Cube.obj", { 0.05, 0.05, 0.65, 1.0f }, { -0.15f, -1.35f, 0.5f }, { 0.75f, 0.25f, 0.75f }, { 0.0f, -21.0f, 0.0f });
		//DrawCube("Assets/Models/model.fbx", { 0.73, 0.73, 0.73, 1.0f }, { -0.15f, -0.928f, 0.5f }, { 0.3f, 0.3f, 0.3f }, { 0.0f, 0.0f, 0.0f });
		
		//DrawCube("Assets/Models/BakerHouse.obj", { 0.9, 0.0, 0.9 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, 0.0f, 0.0f});
		DrawCube("Assets/Models/model.fbx", { 0.95, 0.95, 0.95, 1.0f }, { 0.0f, -1.15f, 0.0f }, { 0.3f, 0.3f, 0.3f }, {0.0f, 0.0f, 0.0f});
		DrawCube("Assets/Models/model.fbx", { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.7f, -1.15f, 0.0f }, { 0.3f, 0.3f, 0.3f }, {0.0f, 21.0f, 0.0f});
		DrawCube("Assets/Models/model.fbx", { 0.0f, 0.0f, 1.0f, 1.0f }, { -0.7f, -1.15f, 0.0f }, { 0.3f, 0.3f, 0.3f }, {0.0f, 21.0f, 0.0f});

		//
		//Entity& pointLight = CreateEntity("Spot Light");
		//AddLight(pointLight.Get<TransformComponent>(), pointLight.CreateComponent<LightComponent>(LightType::SPOT));
	}
	
	Scene::~Scene()
	{

	}
	
	void Scene::Update()
	{
		for (int i = 0; i < world.size(); ++i)
		{
			Entity* entity = world[i];
			if (MeshComponent* mesh = entity->Get<MeshComponent>())
			{
				Renderer::Submit(shader, entity->Get<MaterialComponent>()->GetMaterial(), mesh->GetVertexArray(), entity->Get<TransformComponent>()->GetTransform());
			}
		}
	}

	void Scene::Reset()
	{
		for (int i = world.size() - 1; i >= 0; --i)
		{
			world[i]->Destroy();
		}

		world.clear();
		transforms.clear();
		positions.clear();
		indices.clear();
		normals.clear();
		meshesInfo.clear();
		objectsInfo.clear();
		aabbs.clear();
		lights.clear();
	}

	void Scene::CollectInformation()
	{
		transforms.clear();
		positions.clear();
		indices.clear();
		normals.clear();
		meshesInfo.clear();
		objectsInfo.clear();
		aabbs.clear();
		textures.clear();

		float transformsCount = 0.0f;

		for (int i = 0; i < world.size(); ++i)
		{
			Entity* entity = world[i];
			entity->Get<TransformComponent>()->SetID();
			MeshComponent* comp = entity->Get<MeshComponent>();
			if (comp != nullptr)
			{
				if (comp->GetMesh() != nullptr)
				{
					comp->GetMesh()->SetID();
				}
				const std::shared_ptr<Material>& mat = entity->Get<MaterialComponent>()->GetMaterial();
				mat->SetID();
				mat->ResetIDTextures();
			}
		}

		numTextures = 0;
		for (int i = 0; i < world.size(); ++i)
		{
			Entity* entity = world[i];
			if (MeshComponent* comp = entity->Get<MeshComponent>())
			{
				entity->Get<TransformComponent>()->SetID(transforms.size());
				transforms.push_back(entity->Get<TransformComponent>()->GetTransform());
				std::shared_ptr<Mesh> mesh = comp->GetMesh();
				if (mesh && mesh->GetID() == -1)
				{
					mesh->SetID(meshesInfo.size());

					float offsetVertices = positions.size();
					float sizeVertices = offsetVertices + comp->GetPositions().size();
					positions.resize(sizeVertices);
					std::copy(comp->GetPositions().begin(), comp->GetPositions().end(), &positions[offsetVertices]);		
					
					float offsetIndices = indices.size();
					float sizeIndices = offsetIndices + comp->GetIndices().size();
					indices.resize(sizeIndices);
					std::copy(comp->GetIndices().begin(), comp->GetIndices().end(), &indices[offsetIndices]);

					float offsetNormals = normals.size();
					normals.resize(offsetNormals + comp->GetNormals().size());
					std::copy(comp->GetNormals().begin(), comp->GetNormals().end(), &normals[offsetNormals]);

					meshesInfo.push_back(glm::vec4(offsetVertices, sizeVertices, offsetIndices, sizeIndices));
					aabbs.push_back(mesh->GetAABB());
				}

				ObjectInfo info;
				info.info.x = transforms.size() - 1;
				info.info.y = mesh->GetID();
				info.info.z = materialsInfo.size();
				info.info.w = 0.0f;
				objectsInfo.push_back(info);

				std::shared_ptr<Material> material = entity->Get<MaterialComponent>()->GetMaterial();
				material->SetID(materialsInfo.size());
				material->SetIDTextures(textures, numTextures);
				MaterialInfo matInfo;
				matInfo.textureIDs.x = material->GetDiffuse() ? material->GetDiffuse()->GetImageID() : -1.0f;
				matInfo.textureIDs.y = material->GetNormalMap() ? material->GetNormalMap()->GetImageID() : -1.0f;
				matInfo.textureIDs.z = material->GetMetallicMap() ? material->GetMetallicMap()->GetImageID() : -1.0f;
				matInfo.textureIDs.w = material->GetRoughnessMap() ? material->GetRoughnessMap()->GetImageID() : -1.0f;
				matInfo.color = glm::vec4(material->GetColor(), material->GetAbsorption());
				matInfo.properties.x = material->GetMetallic();
				matInfo.properties.y = material->GetRoughness();
				matInfo.properties.z = material->GetRefractionIndex();
				matInfo.properties.w = material->GetTransmission();
				matInfo.emissive = glm::vec4(material->GetEmissive(), material->GetEmission());

				materialsInfo.push_back(matInfo);
			}
		}
	}
	
	Entity* Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity* Scene::CreateEntityWithPath(const std::string& path, const std::string& name)
	{
		Entity* entity = CreateEntityWithUUID(UUID(), name);
		entity->CreateComponent<MeshComponent>(path);
		entity->CreateComponent<MaterialComponent>();

		return entity;
	}
	
	Entity* Scene::CreateEntityWithUUID(UUID id, const std::string& name)
	{
		Entity* entity = new Entity(id, name);
		world.push_back(entity);
		return entity;
	}

	void Scene::DestroyEntity(Entity* ent)
	{
		for (auto it = world.begin(); it != world.end(); ++it)
		{
			if (*it == ent)
			{
				(*it)->Destroy();
				delete *it;
				world.erase(it);
				break;
			}
		}
	}
	
	void Scene::DrawCube(const std::string& path, const glm::vec4& color, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rot)
	{
		Entity* ent1 = CreateEntity("Entity" + std::to_string(world.size()));
		ent1->CreateComponent<MeshComponent>(path);
		ent1->CreateComponent<MaterialComponent>();

		ent1->Get<MaterialComponent>()->GetMaterial()->GetColor() = color;

		TransformComponent* transform = ent1->Get<TransformComponent>();
		transform->SetPosition(pos);
		transform->SetScale(scale);
		transform->SetRotation(rot);
	}
}