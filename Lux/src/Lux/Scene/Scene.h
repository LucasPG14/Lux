#pragma once

#include "Entity.h"

#include "Lux/Renderer/Shader.h"

namespace Lux
{
	class TransformComponent;
	class LightComponent;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();

		inline std::vector<Entity>& GetWorld() { return world; }

		Entity& CreateEntity(const std::string& name = "Entity");
		Entity& CreateEntityWithUUID(UUID id, const std::string& name = "Entity");
	
		void AddLight(TransformComponent* transform, LightComponent* light) { lights.emplace_back(transform,light); }

		const std::vector<std::pair<TransformComponent*, LightComponent*>>& GetLights() { return lights; }

	private:
		std::vector<Entity> world;

		std::vector<std::pair<TransformComponent*, LightComponent*>> lights;

		std::shared_ptr<Shader> shader;

		friend class SceneSerializer;
		friend class SceneRenderer;
	};
}