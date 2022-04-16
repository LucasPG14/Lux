#pragma once

#include <Amethyst.h>

#include "ImGuiWindows/SceneHierarchyWindow.h"
#include "ImGuiWindows/ContentBrowserWindow.h"

namespace Amethyst
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		~EditorLayer();

		void OnCreate() override;
		void OnDestroy() override;

		void Update(Timer timer) override;
		void RenderImGui() override;

		void OnEvent(Event& e) override;

	private:
		bool ShortCuts(KeyPressedEvent& e);

		void AddToScene(std::filesystem::path& path);

		void NewScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		
		void SaveScene();
		void SaveScene(const std::filesystem::path& path);
	
	private:
		SceneHierarchyWindow hierarchy;
		ContentBrowserWindow contentBrowser;
		
		int guizmoState;
		
		std::shared_ptr<Scene> scene;

		std::shared_ptr<Framebuffer> fbo;

		PerspectiveCamera camera;

		glm::vec2 viewSize;
	};
}