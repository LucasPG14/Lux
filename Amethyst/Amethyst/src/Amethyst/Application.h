#pragma once

#include "Core.h"
#include "Amethyst/LayerStack.h"
#include "Amethyst/Events/Event.h"
#include "Amethyst/Events/ApplicationEvent.h"

#include "Window.h"

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
		bool running;

		LayerStack layerStack;


		static Application* app;
	};

	Application* CreateApp();
}