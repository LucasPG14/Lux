#pragma once

#include "Entity.h"

#include "Amethyst/Renderer/Shader.h"

namespace Amethyst
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update(const std::shared_ptr<Shader>& shader);

		inline std::vector<Entity>& GetWorld() { return world; }

		Entity& CreateEntity(const std::string& name = "Entity");
	
	private:
		std::vector<Entity> world;

		friend class SceneSerializer;
	};
}