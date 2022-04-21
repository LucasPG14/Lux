#include "amtpch.h"
#include "Scene.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/CameraComponent.h"

#include "Amethyst/Renderer/Renderer.h"

// TODO: Remove this
#include <glm/gtc/type_ptr.hpp>

namespace Amethyst
{
	Scene::Scene()
	{
		Entity& entity = CreateEntity("Main Camera");
		entity.CreateComponent<CameraComponent>();
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
				Renderer::Submit(mesh->GetMesh()->GetShader(), mesh->GetMesh()->GetVAO(), entity.Get<MaterialComponent>()->GetMaterial(), entity.Get<TransformComponent>()->GetTransform());
			
				if (entity.GetAABBDebug())
				{
					AABB aabb = mesh->GetMesh()->GetAABB();

					float vertices[24] =
					{
						aabb.min.x, aabb.min.y, aabb.min.z,
						aabb.min.x, aabb.min.y, aabb.max.z,
						aabb.min.x, aabb.max.y, aabb.min.z,
						aabb.min.x, aabb.max.y, aabb.max.z,
						aabb.max.x, aabb.min.y, aabb.min.z,
						aabb.max.x, aabb.min.y, aabb.max.z,
						aabb.max.x, aabb.max.y, aabb.min.z,
						aabb.max.x, aabb.max.y, aabb.max.z,
					};

					std::shared_ptr<VertexArray> vao = VertexArray::Create();
					std::shared_ptr<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(float) * 24);
					
					{
						BufferLayout layout =
						{
							{ShaderDataType::FLOAT3, "position"},
						};

						vbo->SetLayout(layout);
					}

					vao->AddVertexBuffer(vbo);

					uint32_t indices[24] =
					{
						0,1,
						1,3,
						3,2,
						2,0,
				
						1,5,
						4,6,
						7,3,
				
						6,7,
						6,2,
				
						7,5,
						4,5,
						4,0
					};

					std::shared_ptr<IndexBuffer> ebo = IndexBuffer::Create(indices, 24);
					vao->AddIndexBuffer(ebo);

					Renderer::ChangeState(true);

					Renderer::Submit(mesh->GetMesh()->GetShader(), vao, entity.Get<MaterialComponent>()->GetMaterial(), entity.Get<TransformComponent>()->GetTransform());

					Renderer::ChangeState(false);
				}
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