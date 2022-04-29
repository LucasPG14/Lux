#pragma once

#include "Core.h"

#include "Window.h"
#include "Amethyst/Core/LayerStack.h"
#include "Amethyst/Events/Event.h"
#include "Amethyst/Events/ApplicationEvent.h"

#include "Amethyst/Core/Timer.h"

namespace Amethyst
{
	class ImGuiLayer;

	class AMT_API Application
	{
	public:
		Application();

		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline void Close() { running = false; }

		inline static Application& Get() { return *app; }
		inline Window& GetWindow() { return *window; }
	private:
		bool CloseWindow(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> window;
		ImGuiLayer* imguiLayer;
		bool running;

		LayerStack layerStack;
		float lastFrameTime;

		static Application* app;
	};

	Application* CreateApp();
}