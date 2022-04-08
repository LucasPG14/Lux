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

	private:
		std::shared_ptr<Shader> shader;
		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<Scene> scene;

		std::shared_ptr<Framebuffer> fbo;

		std::shared_ptr<Shader> shader2;
		std::shared_ptr<Shader> texture;
		std::shared_ptr<Texture2D> tex;
		std::shared_ptr<Texture2D> logo;
		std::shared_ptr<VertexArray> squareVA;

		PerspectiveCamera camera;

		glm::vec2 viewSize;

		std::filesystem::path currentDir;
	};
}