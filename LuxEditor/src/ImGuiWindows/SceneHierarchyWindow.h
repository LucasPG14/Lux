#pragma once

#include "Lux/Scene/Scene.h"
#include "Lux/Resources/Texture2D.h"

namespace Lux
{
	class SceneHierarchyWindow
	{
	public:
		SceneHierarchyWindow() = default;
		SceneHierarchyWindow(const std::shared_ptr<Scene>& currScene);

		void Render();

		void SetScene(const std::shared_ptr<Scene>& currScene) { scene = currScene; }

		void SetSelected(Entity* entity) { selected = entity; }
		inline Entity* GetSelected() { return selected; }

	private:
		void DrawEntityComponents(Entity& entity);

	private:
		bool focused;

		std::shared_ptr<Scene> scene;
		std::shared_ptr<Texture2D> removeTex;
		Entity* selected;
	};
}