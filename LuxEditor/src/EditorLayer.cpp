#include "EditorLayer.h"

#include "Lux/Utils/FileDialogs.h"
#include "Lux/Utils/Math.h"
#include "Lux/Utils/Primitives.h"

#include "Lux/Scene/Entity.h"

#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Optick/src/optick.h>

namespace Lux
{
	extern const std::filesystem::path assetsDir;

	EditorLayer::EditorLayer() 
		: guizmoState(ImGuizmo::TRANSLATE), samples(0), maxSamples(2), needToUpdate(NeedToUpdate::NONE), sceneChanged(false), startRenderer(false), rendererSize(1920.0, 1080.0)
	{
		scene = CreateSharedPtr<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize("Assets/ShaderToy.scene");
		
		scene->SetPath("Assets/ShaderToy.scene");

		std::filesystem::create_directory("Library");
		
		contentBrowser = ContentBrowserWindow();
		hierarchy = SceneHierarchyWindow(scene);

		lightingPass = CreateSharedPtr<Shader>("Assets/Shaders/PathTracing.glsl");
		viewSceneShader = CreateSharedPtr<Shader>("Assets/Shaders/PathTracingScene.glsl");
		skyboxShader = CreateSharedPtr<Shader>("Assets/Shaders/Skybox.glsl");
		
		defaultShader = CreateSharedPtr<Shader>("Assets/Shaders/Default.glsl");
		computeShader = CreateSharedPtr<ComputeShader>("Assets/Shaders/ComputeShader.glsl");

		scene->CollectInformation();
		{
			TextureSpecification spec;
			spec.format = TextureFormat::FLOAT;

			transformsTexture = CreateSharedPtr<Texture2D>(scene->GetTransforms().data(), sizeof(glm::mat4) / sizeof(glm::vec4) * scene->GetTransforms().size(), spec);
		}
		{
			TextureSpecification spec;
			spec.format = TextureFormat::FLOAT;

			//objectsTexture = CreateSharedPtr<Texture2D>(scene->GetObjectsInfo().data(), /*sizeof(ObjectInfo) / sizeof(glm::vec4) **/ scene->GetObjectsInfo().size(), spec);
		}
		textureArray = CreateSharedPtr<Texture2DArray>("Assets/Textures/bakeHouse.png");

		verticesSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetPositions().data(), sizeof(glm::vec4) * scene->GetPositions().size(), 0);
		indicesSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetIndices().data(), sizeof(glm::vec4) * scene->GetIndices().size(), 1);
		normalsSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetNormals().data(), sizeof(glm::vec4) * scene->GetNormals().size(), 2);
		objectsSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetObjectsInfo().data(), sizeof(glm::vec4) * scene->GetObjectsInfo().size(), 3);
		meshesSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetMeshesInfo().data(), sizeof(glm::vec4) * scene->GetMeshesInfo().size(), 4);
		materialsSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetMaterialsInfo().data(), sizeof(MaterialInfo) * scene->GetMaterialsInfo().size(), 5);
		transformsSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetTransforms().data(), sizeof(glm::mat4) * scene->GetTransforms().size(), 6);
		aabbsSsbo = CreateSharedPtr<ShaderStorageBuffer>(scene->GetAABBs().data(), sizeof(AABB) * scene->GetAABBs().size(), 7);

		Importer::ImportFBX2("Assets/Models/Cube.obj", assetsDir.string());

/*		lightingPass->SetStorageBlock("verticesSSBO", verticesSsbo->GetBindingIndex());
		lightingPass->SetStorageBlock("indicesSSBO", indicesSsbo->GetBindingIndex());
		lightingPass->SetStorageBlock("objectsSSBO", objectsSsbo->GetBindingIndex());	*/	
		
		viewSceneShader->SetStorageBlock("verticesSSBO", verticesSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("indicesSSBO", indicesSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("normalsSSBO", normalsSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("objectsSSBO", objectsSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("meshesSSBO", meshesSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("materialsSSBO", materialsSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("transformsSSBO", transformsSsbo->GetBindingIndex());
		viewSceneShader->SetStorageBlock("aabbsSSBO", aabbsSsbo->GetBindingIndex());

		//textureArray->AddTexture("Assets/Textures/rustediron2_normal.png");
		//textureArray->AddTexture("Assets/Textures/rustediron2_metallic.png");
		//textureArray = CreateSharedPtr<Texture2DArray>(scene->GetWorld()[0].Get<MaterialComponent>()->GetMaterial());

		//textureArray->AddMaterial(scene->GetWorld()[0].Get<MaterialComponent>()->GetMaterial());
		//textureArray->AddTexture("Assets/Textures/rustediron2_basecolor.png");

		std::vector<float> vertices =
		{
			0.5f, 0.5f, -0.5f, // 0
			0.5f, -0.5f, -0.5f, // 1
			-0.5f, -0.5f, -0.5f, // 2
			-0.5f, 0.5f, -0.5f, // 3
			0.5f, 0.5f, 0.5f, // 4
			0.5f, -0.5f, 0.5f, // 5
			-0.5f, -0.5f, 0.5f, // 6
			-0.5f, 0.5f, 0.5f, // 7
		};

		std::vector<uint32_t> indices =
		{
			0, 1, 2,
			2, 3, 0,

			0, 1, 5,
			5, 4, 0,

			7, 6, 5,
			5, 4, 7,

			3, 2, 6,
			6, 7, 3,

			4, 0, 3,
			3, 7, 4,

			1, 5, 6,
			6, 2, 1
		};
		//Importer::ImportFBX(vertices, indices, "Assets/Models/Cube.fbx");
		vao = CreateSharedPtr<VertexArray>();

		vbo = CreateSharedPtr<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
		{
			BufferLayout layout =
			{
				{ShaderDataType::FLOAT3, "position"},
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);
		
		ebo = CreateSharedPtr<IndexBuffer>(indices.data(), indices.size());

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
				FramebufferTextureFormat::RGBA16,
				FramebufferTextureFormat::DEPTH24_STENCIL8
			};

			sceneFramebuffer = CreateSharedPtr<Framebuffer>(spec);
		}

		// Creating viewport framebuffer
		{
			FramebufferSpecification spec;
			spec.width = 1280;
			spec.height = 720;
			spec.swapChainTarget = true;

			spec.attachments.attachments =
			{
				FramebufferTextureFormat::RGBA16,
				FramebufferTextureFormat::DEPTH24_STENCIL8
			};

			viewportFramebuffer = CreateSharedPtr<Framebuffer>(spec);
		}

		// Creating viewport framebuffer
		{
			FramebufferSpecification spec;
			spec.width = 1280;
			spec.height = 720;
			spec.swapChainTarget = true;

			spec.attachments.attachments =
			{
				FramebufferTextureFormat::RGBA16,
				FramebufferTextureFormat::DEPTH24_STENCIL8
			};

			accumulateFramebuffer = CreateSharedPtr<Framebuffer>(spec);
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

		skybox = CreateSharedPtr<TextureCube>(faces);
	}

	void EditorLayer::OnDestroy()
	{
		OPTICK_EVENT("Editor Layer Destroy");
	}

	void EditorLayer::Update(Timer timer)
	{
		OPTICK_EVENT("Editor Layer Update");

		if (!startRenderer)
		{
			bool moving = camera.Update(timer);
			sceneChanged = moving == true ? moving : sceneChanged;
		}
		if (sceneChanged)
			ResetRenderer();

		if (!startRenderer && samples >= 2)
			return;

		PathTracing();
		
		//if (!startRenderer)
		//	PathTracing();
		//else
		//{
		//	PathTracing();

		//	// Path Tracing with Compute Shader
		//	//{
		//	//	computeShader->Bind();
		//	//	computeShader->SetUniformInt("imgOutput", 0);

		//	//	// Uniforms
		//	//	computeShader->SetUniformFloat3("viewPos", camera.GetPosition());
		//	//	computeShader->SetUniformFloat2("canvas", viewSize);
		//	//	computeShader->SetUniformMat4("inverseCamera", glm::inverse(camera.GetProjectionMatrix()* camera.GetViewMatrix()));
		//	//	computeShader->SetUniformInt("samples", samples);

		//	//	// Textures
		//	//	accumulateFramebuffer->BindTextures(1);
		//	//	computeShader->SetUniformInt("accumulateTexture", 1);
		//	//	transformsTexture->Bind(4);
		//	//	computeShader->SetUniformInt("transformsTex", 4);
		//	//	normalsTexture->Bind(8);
		//	//	computeShader->SetUniformInt("normalsTex", 8);

		//	//	computeShader->DispatchCompute();
		//	//	computeShader->Unbind();
		//	//}
		//	//samples++;

		//	// Accumulating for path tracing
		//}
		accumulateFramebuffer->Bind();

		Renderer::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Renderer::Clear();

		defaultShader->Bind();

		viewportFramebuffer->BindTextures();
		//computeShader->BindTexture();
		defaultShader->SetUniformInt("pathColor", 0);
		//
		Renderer::DrawFullscreenQuad();

		//computeShader->UnbindTexture();
		viewportFramebuffer->UnbindTextures();
		defaultShader->Unbind();

		accumulateFramebuffer->Unbind();
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
					scene->Reset();
					scene = CreateSharedPtr<Scene>();
					scene->Changed(Change::OBJECT);
					hierarchy.SetScene(scene);
				}

				if (ImGui::MenuItem("Open scene", "Ctrl + O"))
				{
					std::string filepath = FileDialog::OpenFile("scene (*.scene)\0*.scene\0");
					if (!filepath.empty())
					{
						SceneSerializer serializer(scene);
						serializer.Deserialize(filepath);
						scene->Changed(Change::OBJECT);
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Save scene", "Ctrl + S"))
				{
					if (scene->GetPath().empty())
					{
						SaveSceneAs();
					}
					else
					{
						SaveScene(scene->GetPath());
					}
				}
				if (ImGui::MenuItem("Save scene as...", "Ctrl + Shift + S"))
				{
					SaveSceneAs();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4")) 
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Object"))
			{
				if (ImGui::MenuItem("Create Point Light"))
				{
					Entity* entity = scene->CreateEntity("Point Light");
					LightComponent* light = entity->CreateComponent<LightComponent>(LightType::POINT);
					scene->AddLight(entity->Get<TransformComponent>(), light);
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
		ImGui::Image((void*)accumulateFramebuffer->GetID(), { viewSize.x, viewSize.y }, { 0, 1 }, { 1, 0 });
		if ((size.x != viewSize.x || size.y != viewSize.y))
		{
			viewSize = { size.x, size.y };
			
			computeShader->ResizeTexture(viewSize.x, viewSize.y);
			sceneFramebuffer->Resize(viewSize.x, viewSize.y);
			viewportFramebuffer->Resize(viewSize.x, viewSize.y);
			accumulateFramebuffer->Resize(viewSize.x, viewSize.y);
			camera.SetDimensions(viewSize.x, viewSize.y);
			sceneChanged = true;
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
					//if (realPath.extension().string() == ".bsscene") OpenScene(realPath);
					if (realPath.extension().string() == ".obj" || realPath.extension().string() == ".fbx")
					{
						scene->CreateEntityWithPath(realPath.string(), realPath.filename().string());
						scene->Changed(Change::OBJECT);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		// ImGuizmo Begin
		if (Entity* entitySelected = hierarchy.GetSelected())
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		
			const glm::mat4& camView = glm::transpose(camera.GetViewMatrix());
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

				scene->Changed(Change::TRANSFORM);
			}
		}
		// ImGuizmo End

		// End Mouse Picking

		ImGui::PopStyleVar();
		ImGui::End();
		// Viewport End

		ImGui::Begin("Path Tracing samples");
		ImGui::Text("Samples: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(samples).c_str());
		ImGui::DragInt("Max samples", &maxSamples);

		std::string actualValue = rendererSize == glm::vec2(1920.0, 1080.0) ? "1920 x 1080" : "1080 x 720";
		if (ImGui::BeginCombo("Renderer Size", actualValue.c_str()))
		{
			if (ImGui::Selectable("1920 x 1080"))
			{
				rendererSize = glm::vec2(1920.0, 1080.0);
			}
			if (ImGui::Selectable("1080 x 720"))
			{
				rendererSize = glm::vec2(1080.0, 720.0);
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Start Renderer"))
		{
			startRenderer = true;
			viewportFramebuffer->Resize(rendererSize.x, rendererSize.y);
			accumulateFramebuffer->Resize(rendererSize.x, rendererSize.y);
			camera.SetDimensions(rendererSize.x, rendererSize.y);
			ResetRenderer();
		}

		if (ImGui::Button("Restart"))
		{
			startRenderer = false;
			ResetRenderer();
			viewportFramebuffer->Resize(viewSize.x, viewSize.y);
			accumulateFramebuffer->Resize(viewSize.x, viewSize.y);
			camera.SetDimensions(viewSize.x, viewSize.y);
		}

		if (samples >= maxSamples)
		{
			if (ImGui::Button("Save Renderer"))
			{
				SaveImage();
			}
		}

		ImGui::End();

		hierarchy.Render();
		contentBrowser.Render();

		switch (scene->GetChange())
		{
		case Change::NONE:
			break;
		case Change::TRANSFORM:
		{
			TransformComponent* comp = hierarchy.GetSelected()->Get<TransformComponent>();
			transformsSsbo->ChangeData((void*)glm::value_ptr(comp->GetTransform()), comp->GetID() * sizeof(glm::mat4), sizeof(glm::mat4));
			scene->Changed(Change::NONE);
			sceneChanged = true;
			break;
		}
		case Change::MATERIAL:
		{
			MaterialInfo info;
			const std::shared_ptr<Material>& material = hierarchy.GetSelected()->Get<MaterialComponent>()->GetMaterial();
			info.color = material->GetColor();
			info.properties.x = material->GetMetallic();
			info.properties.y = material->GetRoughness();
			info.properties.z = material->GetRefractionIndex();
			info.properties.w = material->GetTransmission();
			//info.emissive = material->GetEmissive();
			materialsSsbo->ChangeData(&info, material->GetID() * sizeof(MaterialInfo), sizeof(MaterialInfo));

			scene->Changed(Change::NONE);
			sceneChanged = true;
			break;
		}
		case Change::OBJECT:
		{
			scene->CollectInformation();

			verticesSsbo->Reset(scene->GetPositions().data(), sizeof(glm::vec4) * scene->GetPositions().size());
			indicesSsbo->Reset(scene->GetIndices().data(), sizeof(glm::vec4) * scene->GetIndices().size());
			normalsSsbo->Reset(scene->GetNormals().data(), sizeof(glm::vec4) * scene->GetNormals().size());
			objectsSsbo->Reset(scene->GetObjectsInfo().data(), sizeof(glm::vec4) * scene->GetObjectsInfo().size());
			meshesSsbo->Reset(scene->GetMeshesInfo().data(), sizeof(glm::vec4) * scene->GetMeshesInfo().size());
			materialsSsbo->Reset(scene->GetMaterialsInfo().data(), sizeof(MaterialInfo) * scene->GetMaterialsInfo().size());
			transformsSsbo->Reset(scene->GetTransforms().data(), sizeof(glm::mat4) * scene->GetTransforms().size());
			aabbsSsbo->Reset(scene->GetAABBs().data(), sizeof(AABB) * scene->GetAABBs().size());

			scene->Changed(Change::NONE);
			sceneChanged = true;
			break;
		}
		case Change::LIGHT:
		{
			ResetRenderer();

			scene->Changed(Change::NONE);
			sceneChanged = true;
			break;
		}
		}

		ImGui::End();
	}

	void EditorLayer::ResetRenderer()
	{
		samples = 0;

		accumulateFramebuffer->Bind();
		Renderer::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Renderer::Clear();
		accumulateFramebuffer->Unbind();

		sceneChanged = false;
		imageSaved = false;
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		contentBrowser.OnEvent(e);

		dispatcher.Dispatch<KeyPressedEvent>(LUX_BIND_EVENT_FN(EditorLayer::ShortCuts));
	}

	void EditorLayer::PathTracingView()
	{
		viewportFramebuffer->Bind();

		Renderer::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Renderer::Clear();

		viewSceneShader->Bind();

		//transformsTexture->Bind(4);
		//viewSceneShader->SetUniformInt("transformsTex", 4);

		viewSceneShader->SetUniformFloat3("viewPos", camera.GetPosition());

		viewSceneShader->SetUniformFloat3("viewPos", camera.GetPosition());
		viewSceneShader->SetUniformFloat2("canvas", viewSize);
		viewSceneShader->SetUniformMat4("inverseCamera", glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix()));

		Renderer::DrawFullscreenQuad();

		sceneFramebuffer->UnbindTextures();
		viewportFramebuffer->Unbind();
	}

	void EditorLayer::PathTracing()
	{
		if (samples >= maxSamples)
			return;
		//sceneFramebuffer->Bind();

		//Renderer::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		//Renderer::Clear();

		//Renderer::BeginScene(camera);

		//scene->Update();

		//Renderer::EndScene();

		//sceneFramebuffer->Unbind();

		viewportFramebuffer->Bind();

		Renderer::ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Renderer::Clear();

		lightingPass->Bind();

		sceneFramebuffer->BindTextures();
		lightingPass->SetUniformInt("positions", 0);
		lightingPass->SetUniformInt("normals", 1);
		lightingPass->SetUniformInt("albedoSpecular", 2);

		accumulateFramebuffer->BindTextures(3);
		lightingPass->SetUniformInt("accumulateTexture", 3);
		lightingPass->SetUniformInt("samples", samples++);

		transformsTexture->Bind(4);
		lightingPass->SetUniformInt("transformsTex", 4);

		lightingPass->SetUniformInt("numObjects", scene->GetObjectsInfo().size());

		const auto& lights = scene->GetLights();

		int pointLights = 0;
		int spotLights = 0;
		for (int i = 0; i < lights.size(); ++i)
		{
			TransformComponent* transform = lights[i].first;
			LightComponent* light = lights[i].second;
			switch (light->GetType())
			{
			case LightType::DIRECTIONAL:
			{
				lightingPass->SetUniformFloat3("dirLight.direction", transform->GetRotation());
				lightingPass->SetUniformFloat3("dirLight.radiance", light->GetColor());
				break;
			}
			case LightType::POINT:
			{
				std::string number = std::to_string(pointLights++);
				lightingPass->SetUniformFloat3("pointLights[" + number + "].position", transform->GetPosition());
				lightingPass->SetUniformFloat3("pointLights[" + number + "].radiance", light->GetColor());

				lightingPass->SetUniformFloat("pointLights[" + number + "].radius", light->GetRange());
				break;
			}
			case LightType::SPOT:
			{
				std::string number = std::to_string(spotLights++);
				lightingPass->SetUniformFloat3("spotLights[" + number + "].position", transform->GetPosition());
				lightingPass->SetUniformFloat3("spotLights[" + number + "].direction", transform->GetRotation());
				lightingPass->SetUniformFloat3("spotLights[" + number + "].radiance", light->GetColor());

				lightingPass->SetUniformFloat("spotLights[" + number + "].radius", light->GetRange());
				lightingPass->SetUniformFloat("spotLights[" + number + "].cutOff", glm::cos(glm::radians(light->GetCutOff())));
				lightingPass->SetUniformFloat("spotLights[" + number + "].outerCutOff", glm::cos(glm::radians(light->GetCutOff() + 2.0f)));
				break;
			}
			}
		}

		lightingPass->SetUniformFloat3("viewPos", camera.GetPosition());
		lightingPass->SetUniformInt("numPointLights", pointLights);
		lightingPass->SetUniformFloat2("canvas", startRenderer == true ? rendererSize : viewSize);
		lightingPass->SetUniformMat4("inverseCamera", glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix()));

		Renderer::DrawFullscreenQuad();
		//Renderer::DrawSkybox(vao, skybox, skyboxShader, camera.GetViewMatrix(), camera.GetProjectionMatrix());

		sceneFramebuffer->UnbindTextures();
		viewportFramebuffer->Unbind();
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
				if (shift || scene->GetPath().empty())
				{
					// Save Scene As
					SaveSceneAs();
					return true;
				}
				else
				{
					SaveScene(scene->GetPath());
				}
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
		case Keys::DEL:
			if (hierarchy.GetSelected() != nullptr)
			{
				scene->DestroyEntity(hierarchy.GetSelected());
				hierarchy.SetSelected(nullptr);
				scene->Changed(Change::OBJECT);
			}
			break;
		}

		return true;
	}

	void EditorLayer::NewScene()
	{
		scene->Reset();
		scene = CreateSharedPtr<Scene>();
		scene->Changed(Change::OBJECT);
		hierarchy.SetScene(scene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(scene);
			serializer.Deserialize(filepath);
			scene->Changed(Change::OBJECT);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialog::SaveFile("scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			filepath += filepath.find(".") == std::string::npos ? ".scene" : "";
			SaveScene(filepath);
			scene->SetPath(filepath);
		}
	}

	void EditorLayer::SaveScene(const std::string& path)
	{
			SceneSerializer serializer(scene);
			serializer.Serialize(path);
	}
	
	void EditorLayer::SaveImage()
	{
		imageSaved = true;

		std::string filename = FileDialog::SaveFile("png (*.png)\0*.png\0");

		if (!filename.empty())
			accumulateFramebuffer->SaveToFile(filename);
	}
}