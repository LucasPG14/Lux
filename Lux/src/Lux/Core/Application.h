#pragma once

#include "Core.h"

#include "Window.h"
#include "Lux/Core/LayerStack.h"
#include "Lux/Events/Event.h"
#include "Lux/Events/ApplicationEvent.h"

#include "Lux/Core/Timer.h"

namespace Lux
{
	class ImGuiLayer;

	class LUX_API Application
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