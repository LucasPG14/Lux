#pragma once

#include "Scene.h"

namespace Amethyst
{
	class SceneRenderer
	{
	public:
		SceneRenderer(const std::shared_ptr<Scene>& s);
		~SceneRenderer();

		void Render();

	private:
		void GeometryPass();
		void LightingPass();

	private:
		std::shared_ptr<Scene> scene;
	};
}