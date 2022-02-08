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

		vao.reset(VertexArray::Create());

		float vertices[21] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		vbo.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT4, "color"},
				//{ShaderDataType::FLOAT3, "normal"}
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);

		uint32_t indices[3] = { 0, 1, 2 };
		ebo.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		vao->AddIndexBuffer(ebo);

		squareVA.reset(VertexArray::Create());

		float vertices2[12] =
		{
			-0.5f, -0.5f, 0.0f, 
			 0.5f, -0.5f, 0.0f, 
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices2, sizeof(vertices2)));

		BufferLayout layout2 =
		{
			{ShaderDataType::FLOAT3, "position"},
			//{ShaderDataType::FLOAT3, "normal"}
		};

		vertexBuffer->SetLayout(layout2);
		squareVA->AddVertexBuffer(vertexBuffer);

		uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		squareVA->AddIndexBuffer(indexBuffer);

		std::string vertex = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColor;

			out vec3 vPosition;
			out vec4 vColor;
			
			void main()
			{
				vColor = aColor;
				vPosition = aPosition;
				gl_Position = vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 vPosition;
			in vec4 vColor;

			void main()
			{
				color = vec4(vPosition * 0.5 + 0.5, 1.0);
				color = vColor;
			}
		
		)";
		
		shader.reset(new Shader(vertex, fragment));

		std::string vertex2 = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;

			out vec3 vPosition;
			
			void main()
			{
				vPosition = aPosition;
				gl_Position = vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		
		)";

		shader2.reset(new Shader(vertex2, fragment2));
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

			shader2->Bind();
			squareVA->Bind();
			glDrawElements(GL_TRIANGLES, squareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			shader->Bind();
			vao->Bind();
			glDrawElements(GL_TRIANGLES, ebo->GetCount(), GL_UNSIGNED_INT, nullptr);

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