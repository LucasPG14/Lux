#pragma once

#include <Amethyst.h>

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
		bool FileDropped(WindowDropEvent& e);

		bool ShortCuts(KeyPressedEvent& e);

		void AddToScene(std::filesystem::path& path);

		void NewScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		
		void SaveScene();
		void SaveScene(const std::filesystem::path& path);
	private:
		std::filesystem::path currentDir;
		std::filesystem::path assetsDir;
		
		Entity* entSelected;
		
		int guizmoState;
		
		std::shared_ptr<Scene> scene;

		std::shared_ptr<Framebuffer> fbo;

		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> tex;
		std::shared_ptr<Texture2D> folder;

		PerspectiveCamera camera;

		glm::vec2 viewSize;
	};
}