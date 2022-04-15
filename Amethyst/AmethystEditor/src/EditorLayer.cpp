#include "EditorLayer.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include "Amethyst/Utils/Math.h"

#include "Amethyst/Utils/FileDialogs.h"

namespace Amethyst
{
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	EditorLayer::EditorLayer() : currentDir("assets/"), assetsDir("assets/"), entSelected(nullptr), guizmoState(ImGuizmo::TRANSLATE)
	{
		scene = std::make_shared<Scene>();
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnCreate()
	{
		// Creating framebuffer
		FramebufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;
		fbo = Framebuffer::Create(spec);

		// Creating shader
		shader = Shader::Create("assets/shaders/Texture.glsl");

		// Creating textures
		folder = Texture2D::Create("editor/textures/folder.png");
		tex = Texture2D::Create("assets/textures/bakeHouse.png");

		// Binding shader
		shader->Bind();

		// Load Resources
		std::stack<std::filesystem::path> resources;
		resources.push(assetsDir);

		std::string extension = ".bsres";

		while (!resources.empty())
		{
			std::filesystem::path path = resources.top();
			resources.pop();

			for (auto& entry : std::filesystem::directory_iterator(path))
			{
				const std::filesystem::path& filePath = entry.path();
				if (entry.is_directory()) resources.push(entry);
				else if (filePath.extension().string() == extension)
				{
					ResourceSystem::ImportResources(filePath);
				}
			}
		}
	}

	void EditorLayer::OnDestroy()
	{
	}

	void EditorLayer::Update(Timer timer)
	{
		camera.Update(timer);

		fbo->Bind();
		RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderOrder::Clear();

		Renderer::BeginScene(camera);

		scene->Update(shader);

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
				if (ImGui::MenuItem("New scene", "Ctrl + N"))
				{
					NewScene();
				}
				if (ImGui::MenuItem("Open scene", "Ctrl + O"))
				{
					OpenScene();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save scene", "Ctrl + S"))
				{
					// Still not implemented
				}
				if (ImGui::MenuItem("Save scene as...", "Ctrl + Shift + S"))
				{
					SaveScene();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl + Z"))
				{

				}
				if (ImGui::MenuItem("Redo", "Ctrl + Y"))
				{

				}
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "Ctrl + X"))
				{

				}
				if (ImGui::MenuItem("Copy", "Ctrl + C"))
				{

				}
				if (ImGui::MenuItem("Paste", "Ctrl + V"))
				{

				}
				if (ImGui::MenuItem("Duplicate", "Ctrl + D"))
				{

				}
				if (ImGui::MenuItem("Delete", "Del"))
				{

				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// Viewport Begin
		ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoTitleBar;
		static bool viewportEnabled = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

		ImGui::Begin("Viewport", &viewportEnabled, viewportFlags);
		ImVec2 size = ImGui::GetWindowContentRegionMax();
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
					if (realPath.extension().string() == ".bsscene") OpenScene(realPath);
					else AddToScene(realPath);
				}
			}

			ImGui::EndDragDropTarget();
		}

		// ImGuizmo Begin
		if (entSelected)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		
			glm::mat4& camView = glm::transpose(camera.GetViewMatrix());
			const glm::mat4& camProj = glm::transpose(camera.GetProjectionMatrix());

			TransformComponent* tComponent = entSelected->Get<TransformComponent>();
			glm::mat4 transform = glm::transpose(entSelected->Get<TransformComponent>()->GetTransform());

			ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj), (ImGuizmo::OPERATION)guizmoState, ImGuizmo::LOCAL, glm::value_ptr(transform));
		
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position = {}, rotation = {}, scale = {};
				Math::Decompose(transform, position, rotation, scale);

				tComponent->SetPosition(position);
				tComponent->SetRotation(glm::degrees(rotation));
				tComponent->SetScale(scale);
			}
		}
		// ImGuizmo End

		ImGui::PopStyleVar();
		ImGui::End();
		// Viewport End

		// Hierarchy Begin
		ImGui::Begin("Hierarchy");

		std::vector<Entity>& entities = scene->GetWorld();
		for (int i = 0; i < entities.size(); ++i)
		{
			Entity& entity = entities[i];
			ImGui::PushID(i);
			if (ImGui::TreeNodeEx(entity.GetName().c_str(), entSelected == &entity ? ImGuiTreeNodeFlags_Selected : 0))
			{
				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked())
				entSelected = &entity;

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

		if (ImGui::IsWindowFocused())
		{
			if (Input::IsKeyPressed(Keys::DEL))
			{
				std::ifstream file(selected, std::ios::binary);

				uint64_t uuid = 0;
				file.read((char*)&uuid, sizeof(uint64_t));

				ResourceSystem::Delete(uuid);
				std::filesystem::remove_all(selected);
			}
		}

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
			
			if (file.is_directory()) ImGui::ImageButton((ImTextureID)folder->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
			else ImGui::ImageButton((ImTextureID)tex->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
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
					else if (path.extension().string() == ".bsscene") OpenScene(path);
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
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::ShortCuts));
	}
	
	bool EditorLayer::FileDropped(WindowDropEvent& e)
	{
		std::vector<std::string>& paths = e.GetPaths();
		std::string extension = ".png";
		for (int i = 0; i < paths.size(); ++i)
		{
			std::filesystem::path path = paths[i];
			if (path.extension() == extension)
			{
				Importer::ImportTexture(path, currentDir);
				continue;
			}
			Importer::Import(path, currentDir);
		}

		return true;
	}

	bool EditorLayer::ShortCuts(KeyPressedEvent& e)
	{
		bool ctrl = Input::IsKeyPressed(Keys::LEFT_CONTROL) || Input::IsKeyPressed(Keys::RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(Keys::LEFT_SHIFT) || Input::IsKeyPressed(Keys::RIGHT_SHIFT);
		
		switch (e.GetKeyCode())
		{
		case Keys::N:
			if (ctrl)
			{
				// New Scene
				NewScene();
			}
			break;
		case Keys::O:
			if (ctrl)
			{
				// Open Scene
				OpenScene();
			}
			break;
		case Keys::S:
			if (ctrl)
			{
				if (shift)
				{
					// Save Scene As
					SaveScene();
					return true;
				}
				// Save Current Scene
			}
			break;
		case Keys::W:
			guizmoState = ImGuizmo::TRANSLATE;
			break;
		case Keys::E:
			guizmoState = ImGuizmo::ROTATE;
			break;
		case Keys::R:
			guizmoState = ImGuizmo::SCALE;
			break;
		}

		return true;
	}
	
	void EditorLayer::AddToScene(std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);

		std::filesystem::path name = path;
		name.replace_extension("");

		uint64_t uuid;
		file.read((char*)&uuid, sizeof(uint64_t));

		std::uint32_t type;
		file.read((char*)&type, sizeof(std::uint32_t));
		
		switch (type)
		{
		case TypeID<Mesh>::id(): 
		{
			Entity& entity = scene->CreateEntity(name.filename().string());
			entity.CreateComponent<MeshComponent>(ResourceSystem::Get<Mesh>(uuid));

			// Reading the material path
			uint64_t matUUID = 0;
			file.read((char*)&matUUID, sizeof(uint64_t));

			entity.CreateComponent<MaterialComponent>(ResourceSystem::Get<Material>(matUUID));
			entSelected = nullptr;
			break;
		}
		case TypeID<Material>::id():
		{
			break;
		}
		}

		file.close();
	}
	
	void EditorLayer::NewScene()
	{
		scene = std::make_shared<Scene>();
	}
	
	void EditorLayer::OpenScene()
	{
		std::string path = OpenFile("Black Sapphire Scene (*.bsscene)\0*.bsscene\0");

		if (!path.empty())
			OpenScene(path);
	}
	
	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (path.extension().string() != ".bsscene")
		{
			AMT_WARN("This isn't a Black Sapphire Scene");
			return;
		}

		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);

		// If the scene is well deserialized, then copy into the active scene
		if (serializer.Deserialize(path))
		{
			scene = newScene;
		}
	}
	
	void EditorLayer::SaveScene()
	{
		std::string path = SaveFile("Black Sapphire Scene (*.bsscene)\0*.bsscene\0");

		if (!path.empty())
			SaveScene(path);
	}
	
	void EditorLayer::SaveScene(const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path);
	}
}