#include "amtpch.h"
#include "Application.h"

#include "Amethyst/Input.h"
#include "Amethyst/KeyCodes.h"

#include <glad/glad.h>

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

		// Creating ImGuiLayer
		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);

		// TEMPORARY
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float vertices[9] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		unsigned int indices[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		std::string vertex = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;

			out vec3 vPosition;
			
			void main()
			{
				vPosition = aPosition;
				gl_Position = vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			void main()
			{
				color = vec4(vPosition * 0.5 + 0.5, 1.0);
			}
		
		)";
		
		shader.reset(new Shader(vertex, fragment));
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		while (running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			shader->Bind();
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);

			for (Layer* layer : layerStack)
				layer->Update();

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