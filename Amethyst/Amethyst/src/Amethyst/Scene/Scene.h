#pragma once

#include "Entity.h"

namespace Amethyst
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();

		inline std::vector<Entity>& GetWorld() { return world; }

		Entity& CreateEntity(const std::string& name = "Entity");
	
	private:
		std::vector<Entity> world;
	};
}