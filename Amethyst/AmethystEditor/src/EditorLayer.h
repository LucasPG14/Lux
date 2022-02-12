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
		std::shared_ptr<Amethyst::Shader> shader;
		std::shared_ptr<Amethyst::VertexArray> vao;

		std::shared_ptr<Amethyst::Framebuffer> fbo;

		std::shared_ptr<Amethyst::Shader> shader2;
		std::shared_ptr<Amethyst::Shader> texture;
		std::shared_ptr<Amethyst::Texture2D> tex;
		std::shared_ptr<Amethyst::Texture2D> logo;
		std::shared_ptr<Amethyst::VertexArray> squareVA;

		Amethyst::PerspectiveCamera camera;

		glm::vec2 viewSize;
	};
}