#pragma once

#include <Lux.h>

#include "ImGuiWindows/SceneHierarchyWindow.h"
#include "ImGuiWindows/ContentBrowserWindow.h"

namespace Lux
{
	enum class SceneState
	{
		EDITOR = 0,
		RUNTIME = 1
	};

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

		void NewScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		
		void SaveScene();
		void SaveScene(const std::filesystem::path& path);

		void PlayScene();
		void StopScene();
	
	private:
		SceneHierarchyWindow hierarchy;
		ContentBrowserWindow contentBrowser;
		
		int guizmoState;
		SceneState sceneState;
		
		std::shared_ptr<Scene> scene;

		std::shared_ptr<Framebuffer> sceneFramebuffer;
		std::shared_ptr<Framebuffer> viewportFramebuffer;
		std::shared_ptr<Shader> lightningPass;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;

		PerspectiveCamera camera;

		glm::vec2 viewSize;
	};
}