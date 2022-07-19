#include "EditorLayer.h"

#include "glm/gtc/type_ptr.hpp"
#include <ImGui/imgui.h>
#include "ImGuizmo/ImGuizmo.h"

#include "Lux/Utils/FileDialogs.h"
#include "Lux/Utils/Math.h"
#include "Lux/Utils/Primitives.h"

#include <glm/gtx/intersect.hpp>
#include <Optick/src/optick.h>

namespace Lux
{
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	extern const std::filesystem::path assetsDir;

	EditorLayer::EditorLayer() : guizmoState(ImGuizmo::TRANSLATE), sceneState(SceneState::EDITOR)
	{
		scene = std::make_shared<Scene>();
		contentBrowser = ContentBrowserWindow();
		hierarchy = SceneHierarchyWindow(scene);

		lightningPass = Shader::Create("Assets/Shaders/Quad.glsl");
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnCreate()
	{
		OPTICK_EVENT("Editor Layer Create");

		// Creating scene framebuffer
		{
			FramebufferSpecification spec;
			spec.width = 1280;
			spec.height = 720;
			spec.swapChainTarget = true;

			spec.attachments.attachments =
			{
				FramebufferTextureFormat::RGBA16,
				FramebufferTextureFormat::RGBA16,
				FramebufferTextureFormat::RGBA8,
				FramebufferTextureFormat::DEPTH24_STENCIL8
			};

			sceneFramebuffer = Framebuffer::Create(spec);
		}

		// Creating viewport framebuffer
		{
			FramebufferSpecification spec;
			spec.width = 1280;
			spec.height = 720;
			spec.swapChainTarget = true;

			spec.attachments.attachments =
			{
				FramebufferTextureFormat::RGBA8,
				FramebufferTextureFormat::DEPTH24_STENCIL8
			};

			viewportFramebuffer = Framebuffer::Create(spec);
		}
	}

	void EditorLayer::OnDestroy()
	{
		OPTICK_EVENT("Editor Layer Destroy");
	}

	void EditorLayer::Update(Timer timer)
	{
		OPTICK_EVENT("Editor Layer Update");

		camera.Update(timer);

		sceneFramebuffer->Bind();
		RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderOrder::Clear();

		Renderer::BeginScene(camera);

		scene->Update();

		Renderer::EndScene();

		sceneFramebuffer->Unbind();

		viewportFramebuffer->Bind();
		RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderOrder::Clear();

		lightningPass->Bind();
		sceneFramebuffer->BindTextures();
		lightningPass->UploadUniformInt("positions", 0);
		lightningPass->UploadUniformInt("normals", 1);
		lightningPass->UploadUniformInt("albedoSpecular", 2);

		const auto& lights = scene->GetLights();

		for (int i = 0; i < lights.size(); ++i)
		{
			LightComponent* light = lights[i].second;
			switch (light->GetType())
			{
			case LightType::DIRECTIONAL:
				lightningPass->UploadUniformFloat3("dirLight.direction", lights[i].first->GetRotation());
				lightningPass->UploadUniformFloat3("dirLight.ambient", light->GetColor());
				lightningPass->UploadUniformFloat3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				lightningPass->UploadUniformFloat3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			case LightType::POINT:
				lightningPass->UploadUniformFloat3("pointLight.position", lights[i].first->GetRotation());
				lightningPass->UploadUniformFloat3("pointLight.ambient", light->GetColor());
				lightningPass->UploadUniformFloat3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				lightningPass->UploadUniformFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			}
		}
		
		Renderer::DrawQuad();

		viewportFramebuffer->Unbind();
	}

	void EditorLayer::RenderImGui()
	{
		OPTICK_EVENT("Editor Layer Render");

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
					NewScene();

				if (ImGui::MenuItem("Open scene", "Ctrl + O"))
					OpenScene();

				ImGui::Separator();
				if (ImGui::MenuItem("Save scene", "Ctrl + S"))
				{
					// Still not implemented
				}
				if (ImGui::MenuItem("Save scene as...", "Ctrl + Shift + S"))
					SaveScene();

				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4")) 
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Object"))
			{
				if (ImGui::MenuItem("Create Directional Light"))
				{
					Entity& entity = scene->CreateEntity("Light");
					LightComponent* light = entity.CreateComponent<LightComponent>(LightType::DIRECTIONAL);
					scene->AddLight(entity.Get<TransformComponent>(), light);
				}

				if (ImGui::MenuItem("Create Point Light"))
				{
					Entity& entity = scene->CreateEntity("Light");
					LightComponent* light = entity.CreateComponent<LightComponent>(LightType::POINT);
					scene->AddLight(entity.Get<TransformComponent>(), light);
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
		ImGui::Image((ImTextureID)viewportFramebuffer->GetID(), { viewSize.x, viewSize.y }, { 0, 1 }, { 1, 0 });
		if (size.x != viewSize.x || size.y != viewSize.y)
		{
			viewSize = { size.x, size.y };
			sceneFramebuffer->Resize(viewSize.x, viewSize.y);
			camera.SetDimensions(viewSize.x, viewSize.y);
		}

		// DragAndDrop Target
		if (ImGui::BeginDragDropTarget())
		{
			//if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
			//{
			//	const wchar_t* path = (const wchar_t*)payload->Data;
			//	std::filesystem::path realPath = std::filesystem::path(assetsDir) / path;
			//	if (realPath.has_extension())
			//	{
			//		if (realPath.extension().string() == ".bsscene") OpenScene(realPath);
			//	}
			//}

			ImGui::EndDragDropTarget();
		}

		// ImGuizmo Begin
		if (Entity* entitySelected = hierarchy.GetSelected())
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		
			glm::mat4& camView = glm::transpose(camera.GetViewMatrix());
			const glm::mat4& camProj = glm::transpose(camera.GetProjectionMatrix());

			TransformComponent* tComponent = entitySelected->Get<TransformComponent>();
			glm::mat4 transform = entitySelected->Get<TransformComponent>()->GetTransform();

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

		// Begin Mouse Picking
		if (ImGui::IsWindowFocused())
		{
			if (Input::IsMouseButtonPressed(Mouse::BUTTON_LEFT))
			{
			}
		}

		// End Mouse Picking

		ImGui::PopStyleVar();
		ImGui::End();
		// Viewport End

		hierarchy.Render();
		contentBrowser.Render();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		contentBrowser.OnEvent(e);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::ShortCuts));
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
			LUX_WARN("This isn't a Black Sapphire Scene");
			return;
		}

		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);

		// If the scene is well deserialized, then copy into the active scene
		if (serializer.Deserialize(path))
		{
			scene = newScene;
			hierarchy.SetScene(scene);
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
	
	void EditorLayer::PlayScene()
	{
		sceneState = SceneState::RUNTIME;
		ImGui::StyleColorsClassic();
		SaveScene("Editor/Scenes/ScenePlay.bsscene");
	}
	
	void EditorLayer::StopScene()
	{
		sceneState = SceneState::EDITOR;
		ImGui::StyleColorsDark();
		OpenScene("Editor/Scenes/ScenePlay.bsscene");
		hierarchy.SetSelected(nullptr);
	}
}