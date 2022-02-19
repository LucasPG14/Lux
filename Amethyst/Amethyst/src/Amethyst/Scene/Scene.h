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

		inline std::vector<Entity>& GetEntities() { return entities; }
	private:
		std::vector<Entity> entities;
	};
}