#include "EditorLayer.h"

#include "imgui/imgui.h"

namespace Amethyst
{
	EditorLayer::EditorLayer()
	{
		scene = std::make_shared<Scene>();
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnCreate()
	{
		Amethyst::FramebufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;
		fbo = Amethyst::Framebuffer::Create(spec);

		vao.reset(Amethyst::VertexArray::Create());

		float vertices[21] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Amethyst::VertexBuffer> vbo;
		vbo.reset(Amethyst::VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			Amethyst::BufferLayout layout =
			{
				{Amethyst::ShaderDataType::FLOAT3, "position"},
				{Amethyst::ShaderDataType::FLOAT4, "color"},
				//{ShaderDataType::FLOAT3, "normal"}
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Amethyst::IndexBuffer> ebo;
		ebo.reset(Amethyst::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		vao->AddIndexBuffer(ebo);

		squareVA.reset(Amethyst::VertexArray::Create());

		float vertices2[20] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Amethyst::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Amethyst::VertexBuffer::Create(vertices2, sizeof(vertices2)));

		Amethyst::BufferLayout layout2 =
		{
			{Amethyst::ShaderDataType::FLOAT3, "position"},
			{Amethyst::ShaderDataType::FLOAT2, "texCoord"},
			//{ShaderDataType::FLOAT3, "normal"}
		};

		vertexBuffer->SetLayout(layout2);
		squareVA->AddVertexBuffer(vertexBuffer);

		uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Amethyst::IndexBuffer> indexBuffer;
		indexBuffer.reset(Amethyst::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		squareVA->AddIndexBuffer(indexBuffer);

		std::string vertex = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColor;

			uniform mat4 view;
			uniform mat4 projection;
			uniform mat4 model;

			out vec4 vColor;
			
			void main()
			{
				vColor = aColor;
				gl_Position = projection * view * model * vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 vColor;

			void main()
			{
				color = vColor;
			}
		
		)";

		shader = Amethyst::Shader::Create("Hola", vertex, fragment);

		std::string vertex2 = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;

			uniform mat4 view;
			uniform mat4 projection;
			uniform mat4 model;			

			out vec3 vPosition;
			
			void main()
			{
				vPosition = aPosition;
				gl_Position = projection * view * model * vec4(aPosition, 1.0);
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

		shader2 = Amethyst::Shader::Create("Hola2", vertex2, fragment2);

		texture = Amethyst::Shader::Create("assets/shaders/Texture.glsl");

		logo.reset(Amethyst::Texture2D::Create("assets/textures/deadpool.png"));
		tex.reset(Amethyst::Texture2D::Create("assets/textures/bakeHouse.png"));

		texture->Bind();
		texture->UploadUniformInt("ourTexture", 0);
	}

	void EditorLayer::OnDestroy()
	{
	}

	void EditorLayer::Update(Timer timer)
	{
		camera.Update(timer);

		//AMT_INFO("ExampleLayer::Update");
		glm::mat4 model(1.0f);

		fbo->Bind();
		Amethyst::RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Amethyst::RenderOrder::Clear();

		Amethyst::Renderer::BeginScene();

		tex->Bind();
		texture->Bind();
		texture->UploadUniformMat4("view", camera.GetViewMatrix());
		texture->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		texture->UploadUniformMat4("model", model);
		Amethyst::Renderer::Submit(squareVA);
		logo->Bind();
		Amethyst::Renderer::Submit(squareVA);
		//shader->Bind();
		//shader->UploadUniformMat4("view", camera.GetViewMatrix());
		//shader->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		//shader->UploadUniformMat4("model", model);
		//Amethyst::Renderer::Submit(vao);

		Amethyst::Renderer::EndScene();

		fbo->Unbind();
	}

	void EditorLayer::RenderImGui()
	{
		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID id = ImGui::GetID("Dockspace");
			ImGui::DockSpace(id, { 0.0f, 0.0f }, dockspaceFlags);
		}

		// Main Menu Bar
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New scene", "Ctrl + N"));
				if (ImGui::MenuItem("Open scene", "Ctrl + O"));
				ImGui::Separator();
				if (ImGui::MenuItem("Save scene", "Ctrl + S"));
				if (ImGui::MenuItem("Save scene as...", "Ctrl + Shift + S"));
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4")) Amethyst::Application::Get().Close();

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoTitleBar;
		static bool viewportEnabled = true;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

		ImGui::Begin("Viewport", &viewportEnabled, viewportFlags);
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)fbo->GetID(), { viewSize.x, viewSize.y }, { 0, 1 }, { 1, 0 });
		if (size.x != viewSize.x || size.y != viewSize.y)
		{
			viewSize = { size.x, size.y };
			fbo->Resize(viewSize.x, viewSize.y);
			camera.SetDimensions(viewSize.x, viewSize.y);
		}
		ImGui::End();
		ImGui::PopStyleVar();
		//style.WindowMinSize.x = minWinSizeX;

		ImGui::Begin("Hierarchy");
		std::vector<Entity> entities = scene->GetEntities();
		for (int i = 0; i < entities.size(); ++i)
		{
			ImGui::PushID(i);
			if (ImGui::TreeNodeEx(entities[i].GetName().c_str()))
			{
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
	}
}