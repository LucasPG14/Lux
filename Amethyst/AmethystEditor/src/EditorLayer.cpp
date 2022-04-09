#include "EditorLayer.h"

#include "imgui/imgui.h"

namespace Amethyst
{
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	EditorLayer::EditorLayer() : currentDir("assets/"), assetsDir("assets/")
	{
		scene = std::make_shared<Scene>();
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnCreate()
	{
		FramebufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;
		fbo = Framebuffer::Create(spec);

		vao.reset(VertexArray::Create());

		float vertices[21] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vbo;
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
		std::shared_ptr<IndexBuffer> ebo;
		ebo.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		vao->AddIndexBuffer(ebo);

		squareVA.reset(VertexArray::Create());

		//float vertices2[20] =
		//{
		//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		//	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		//	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		//};

		/*std::vector<Vertex> vertices2;
		std::vector<uint32_t> indices2;

		Importer::Import(std::filesystem::path("assets/Resources/Models/BakerHouse.obj"), std::filesystem::path(""), vertices2, indices2);*/

		//std::shared_ptr<VertexBuffer> vertexBuffer;
		//vertexBuffer.reset(VertexBuffer::Create(vertices2.data(), sizeof(Vertex) * vertices2.size()));

		//BufferLayout layout2 =
		//{
		//	{ShaderDataType::FLOAT3, "position"},
		//	{ShaderDataType::FLOAT2, "texCoord"},
		//	//{ShaderDataType::FLOAT3, "normal"}
		//};

		//vertexBuffer->SetLayout(layout2);
		//squareVA->AddVertexBuffer(vertexBuffer);

		//uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		//std::shared_ptr<IndexBuffer> indexBuffer;
		//indexBuffer.reset(IndexBuffer::Create(indices2.data(), indices2.size()));
		//squareVA->AddIndexBuffer(indexBuffer);

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

		shader = Shader::Create("Hola", vertex, fragment);

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

		shader2 = Shader::Create("Hola2", vertex2, fragment2);

		texture = Shader::Create("assets/shaders/Texture.glsl");

		logo.reset(Texture2D::Create("assets/textures/deadpool.png"));
		tex.reset(Texture2D::Create("assets/textures/bakeHouse.png"));

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
		RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderOrder::Clear();

		Renderer::BeginScene();

		tex->Bind();
		texture->Bind();
		texture->UploadUniformMat4("view", camera.GetViewMatrix());
		texture->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		texture->UploadUniformMat4("model", model);

		for (int i = 0; i < scene->GetWorld().size(); ++i)
			scene->GetWorld()[i].Update();
		//Renderer::Submit(squareVA);
		//logo->Bind();
		//Renderer::Submit(squareVA);
		//shader->Bind();
		//shader->UploadUniformMat4("view", camera.GetViewMatrix());
		//shader->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		//shader->UploadUniformMat4("model", model);
		//Amethyst::Renderer::Submit(vao);

		Renderer::EndScene();

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
				if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl + Z"));
				if (ImGui::MenuItem("Redo", "Ctrl + Y"));
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "Ctrl + X"));
				if (ImGui::MenuItem("Copy", "Ctrl + C"));
				if (ImGui::MenuItem("Paste", "Ctrl + V"));
				if (ImGui::MenuItem("Duplicate", "Ctrl + D"));
				if (ImGui::MenuItem("Delete", "Del"));

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoTitleBar;
		static bool viewportEnabled = true;
		

		// Viewport Begin
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

		// DragAndDrop Target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path realPath = std::filesystem::path(assetsDir) / path;
				if (realPath.has_extension())
				{
					MeshComponent* mesh = Importer::Load(realPath);

					Entity& entity = scene->CreateEntity();
					entity.AddComponent(mesh);
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleVar();
		ImGui::End();
		// Viewport End

		// Hierarchy Begin
		ImGui::Begin("Hierarchy");
		static Entity* entSelected;
		std::vector<Entity>& entities = scene->GetWorld();
		for (int i = 0; i < entities.size(); ++i)
		{
			//entities.get<InfoComponent>(entities()[i]);
			ImGui::PushID(i);
			if (ImGui::TreeNodeEx(entities[i].GetName().c_str(), entSelected == &entities[i] ? ImGuiTreeNodeFlags_Selected : 0))
			{
				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked())
				entSelected = &entities[i];

			ImGui::PopID();
		}

		ImGui::End();
		// Hierarchy End

		// Inspector begin
		ImGui::Begin("Inspector");

		if (entSelected)
		{
			entSelected->DrawInspector();
		}

		ImGui::End();
		// Inspector end

		// Content Browser begin
		static bool ret = true;
		ImGui::Begin("Content Browser", &ret, ImGuiWindowFlags_MenuBar);

		ImGui::BeginMenuBar();

		ImGui::EndMenuBar();

		static std::filesystem::path selected;

		// Setting the number of columns
		float cell = 128;

		float width = ImGui::GetContentRegionAvail().x;
		int columns = (int)(width / cell);
		
		if (columns < 1)
			columns = 1;

		ImGui::Columns(columns, 0, false);

		int i = 0;
		for (const auto& file : std::filesystem::directory_iterator(currentDir))
		{
			ImGui::PushID(i++);
			const auto& path = file.path();
			auto relativePath = std::filesystem::relative(file.path(), assetsDir);
			std::string filename = relativePath.filename().string();
			
			ImGui::ImageButton((ImTextureID)tex->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
			// Drag objects from the Content Browser
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* filePath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER", filePath, (wcslen(filePath) + 1) * sizeof(const wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					if (file.is_directory()) currentDir /= path.filename();
				}
				else if (ImGui::IsMouseClicked(0))
				{
					selected = file.path();
				}
			}
			
			ImGui::TextWrapped(filename.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::End();
		// Content Browser end

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowDropEvent>(BIND_EVENT_FN(EditorLayer::FileDropped));
	}
	
	bool EditorLayer::FileDropped(WindowDropEvent& e)
	{
		std::vector<std::string>& paths = e.GetPaths();
		for (int i = 0; i < paths.size(); ++i)
		{
			Importer::Import(std::filesystem::path(paths[i]), currentDir);
		}

		return true;
	}
}