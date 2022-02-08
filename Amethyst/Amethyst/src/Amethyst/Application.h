#pragma once

#include "Core.h"

#include "Window.h"
#include "Amethyst/LayerStack.h"
#include "Amethyst/Events/Event.h"
#include "Amethyst/Events/ApplicationEvent.h"

#include "Amethyst/ImGui/ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

namespace Amethyst
{
	class AMT_API Application
	{
	public:
		Application();

		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *app; }
		inline Window& GetWindow() { return *window; }
	private:
		bool CloseWindow(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> window;
		ImGuiLayer* imguiLayer;
		bool running;

		LayerStack layerStack;

		std::shared_ptr<Shader> shader;
		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;

		std::shared_ptr<Shader> shader2;
		std::shared_ptr<VertexArray> squareVA;

		static Application* app;
	};

	Application* CreateApp();
}