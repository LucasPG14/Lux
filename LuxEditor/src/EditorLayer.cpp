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
	extern const std::filesystem::path assetsDir;

	EditorLayer::EditorLayer() : guizmoState(ImGuizmo::TRANSLATE)
	{
		scene = std::make_shared<Scene>();
		contentBrowser = ContentBrowserWindow();
		hierarchy = SceneHierarchyWindow(scene);

		lightingPass = Shader::Create("Assets/Shaders/Quad.glsl");
		skyboxShader = Shader::Create("Assets/Shaders/Skybox.glsl");

		float vertices[] =
		{
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
		};

		vao = VertexArray::Create();

		vbo = VertexBuffer::Create(vertices, 24 * sizeof(float));
		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"}
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);

		std::vector<uint32_t> indices =
		{
			0, 1, 3, 
			3, 1, 2, 
			2, 6, 7, 
			7, 3, 2, 
			7, 6, 5, 
			5, 4, 7, 
			5, 1, 4, 
			4, 1, 0, 
			4, 3, 7, 
			3, 4, 0, 
			5, 6, 2, 
			5, 1, 2
		};
		ebo = IndexBuffer::Create(indices.data(), indices.size());

		vao->AddIndexBuffer(ebo);
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

		std::vector<std::string> faces = 
		{
				"Assets/Textures/right.jpg",
				"Assets/Textures/left.jpg",
				"Assets/Textures/top.jpg",
				"Assets/Textures/bottom.jpg",
				"Assets/Textures/front.jpg",
				"Assets/Textures/back.jpg"
		};

		skybox = TextureCube::Create(faces);
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
		RenderOrder::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderOrder::Clear();

		Renderer::BeginScene(camera);

		scene->Update();

		//Renderer::DrawSkybox(vao, skybox, skyboxShader, camera.GetViewMatrix(), camera.GetProjectionMatrix());

		Renderer::EndScene();

		sceneFramebuffer->Unbind();

		viewportFramebuffer->Bind();
		RenderOrder::ClearColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		RenderOrder::Clear();

		lightingPass->Bind();
		sceneFramebuffer->BindTextures();
		lightingPass->UploadUniformInt("positions", 0);
		lightingPass->UploadUniformInt("normals", 1);
		lightingPass->UploadUniformInt("albedoSpecular", 2);

		const auto& lights = scene->GetLights();

		int pointLights = 0;
		for (int i = 0; i < lights.size(); ++i)
		{
			LightComponent* light = lights[i].second;
			switch (light->GetType())
			{
			case LightType::DIRECTIONAL:
				lightingPass->UploadUniformFloat3("dirLight.direction", lights[i].first->GetRotation());
				lightingPass->UploadUniformFloat3("dirLight.ambient", light->GetColor());
				lightingPass->UploadUniformFloat3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				lightingPass->UploadUniformFloat3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			case LightType::POINT:
				std::string number = std::to_string(pointLights);
				lightingPass->UploadUniformFloat3("pointLights[" + number + "].position", lights[i].first->GetPosition());
				lightingPass->UploadUniformFloat3("pointLights[" + number + "].ambient", light->GetColor());
				lightingPass->UploadUniformFloat3("pointLights[" + number + "].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				lightingPass->UploadUniformFloat3("pointLights[" + number + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
				
				float lightMax = std::fmaxf(std::fmaxf(light->GetColor().r, light->GetColor().g), light->GetColor().b);
				float radius = (-0.7f + std::sqrtf(0.7f * 0.7f - 4 * 1.8f * (1.0f - (256.0f / 5.0f) * lightMax))) / (2 * 1.8f);
				lightingPass->UploadUniformFloat("pointLights[" + number + "].radius", radius);

				float length = glm::length(lights[i].first->GetPosition() - glm::vec3(0.0f, 0.0f, 0.0f));

				pointLights++;
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
				{

				}

				if (ImGui::MenuItem("Open scene", "Ctrl + O"))
				{

				}

				ImGui::Separator();
				if (ImGui::MenuItem("Save scene", "Ctrl + S"))
				{
					// Still not implemented
				}
				if (ImGui::MenuItem("Save scene as...", "Ctrl + Shift + S"))
				{

				}

				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4")) 
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Object"))
			{
				if (ImGui::MenuItem("Create Directional Light"))
				{
					Entity& entity = scene->CreateEntity("Directional Light");
					LightComponent* light = entity.CreateComponent<LightComponent>(LightType::DIRECTIONAL);
					scene->AddLight(entity.Get<TransformComponent>(), light);
				}

				if (ImGui::MenuItem("Create Point Light"))
				{
					Entity& entity = scene->CreateEntity("Point Light");
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

		dispatcher.Dispatch<KeyPressedEvent>(LUX_BIND_EVENT_FN(EditorLayer::ShortCuts));
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
				
			}
			break;
		case Keys::O:
			if (ctrl)
			{
				// Open Scene
				
			}
			break;
		case Keys::S:
			if (ctrl)
			{
				if (shift)
				{
					// Save Scene As
					
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
}