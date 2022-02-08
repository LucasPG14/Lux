#pragma once

#include "Core.h"

#include "Window.h"
#include "Amethyst/LayerStack.h"
#include "Amethyst/Events/Event.h"
#include "Amethyst/Events/ApplicationEvent.h"

#include "Amethyst/ImGui/ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"

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

		unsigned int vao;
		std::unique_ptr<Shader> shader;
		std::unique_ptr<VertexBuffer> vbo;
		std::unique_ptr<IndexBuffer> ebo;

		static Application* app;
	};

	Application* CreateApp();
}