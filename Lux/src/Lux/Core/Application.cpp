#include "luxpch.h"
#include "Application.h"

#include "Lux/ImGui/ImGuiLayer.h"

#include "Lux/Renderer/Renderer.h"

#include "Lux/Core/Input.h"

#include <GLFW/glfw3.h>
#include <optick.h>

namespace Lux
{
	Application* Application::app = nullptr;

	Application::Application() : running(true)
	{
		LUX_CORE_ASSERT(!app, "Application already created!");
		app = this;
		// Creating Window
		window = std::unique_ptr<Window>(Window::Create());
		window->SetEventCallback(LUX_BIND_EVENT_FN(Application::OnEvent));

		// Initializing renderer
		Renderer::Init();

		// Creating ImGuiLayer
		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);
	}
	
	Application::~Application()
	{
		for (Layer* layer : layerStack)
			layer->OnDestroy();
	}
	
	void Application::Run()
	{
		while (running)
		{
			OPTICK_FRAME("MainThread");

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

		dispatcher.Dispatch<WindowCloseEvent>(LUX_BIND_EVENT_FN(Application::CloseWindow));

		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			if (e.handled)
				break;
			(*--it)->OnEvent(e);
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
		Close();
		return true;
	}
}