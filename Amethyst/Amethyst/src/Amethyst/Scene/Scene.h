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
	
	private:
		std::vector<Entity> world;
	};
}