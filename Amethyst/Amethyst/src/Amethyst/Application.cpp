#include "amtpch.h"
#include "Application.h"

#include "Amethyst/Renderer/Renderer.h"

#include "Amethyst/Input.h"

#include <GLFW/glfw3.h>

namespace Amethyst
{
	Application* Application::app = nullptr;

	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application() : running(true)
	{
		AMT_CORE_ASSERT(!app, "Application already created!");
		app = this;
		// Creating Window
		window = std::unique_ptr<Window>(Window::Create());
		window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		// Initializing renderer
		Renderer::Init();

		// Creating ImGuiLayer
		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		while (running)
		{
			float time = (float)glfwGetTime();
			Timer timer = time - lastFrameTime;
			lastFrameTime = time;

			for (Layer* layer : layerStack)
				layer->Update(timer);

			// This should be on the renderer, on a separate thread
			imguiLayer->Begin();

			for (Layer* layer : layerStack)
				layer->RenderImGui();

			imguiLayer->End();

			window->Update();
		}
	}
	
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::CloseWindow));

		AMT_CORE_TRACE("{0}", e);

		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnCreate();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnCreate();
	}

	bool Application::CloseWindow(WindowCloseEvent& e)
	{
		running = false;
		return true;
	}
}