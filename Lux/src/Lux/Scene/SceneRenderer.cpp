#include "luxpch.h"
#include "SceneRenderer.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"

#include "Lux/Renderer/Renderer.h"

namespace Lux
{
	SceneRenderer::SceneRenderer(const std::shared_ptr<Scene>& s) : scene(s)
	{
	}
	
	SceneRenderer::~SceneRenderer()
	{
	}
	
	void SceneRenderer::Render()
	{
		GeometryPass();
		// Here we should render the skybox
		LightingPass();
	}
	
	void SceneRenderer::GeometryPass()
	{
		for (Entity& entity : scene->world)
		{
			if (MeshComponent* mesh = entity.Get<MeshComponent>())
			{
				MaterialComponent* material = entity.Get<MaterialComponent>();
				Renderer::Submit(scene->shader, material->GetMaterial(), mesh->GetVertexArray(), entity.Get<TransformComponent>()->GetTransform());
			}
		}
	}
	
	void SceneRenderer::LightingPass()
	{
	}
}