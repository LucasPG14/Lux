#pragma once

#include <Lux.h>

#include "ImGuiWindows/SceneHierarchyWindow.h"
#include "ImGuiWindows/ContentBrowserWindow.h"

namespace Lux
{
	enum class NeedToUpdate
	{
		NONE = 0,
		TRANSFORMS = 1,
		MATERIALS = 2,
		TEXTURES = 3,
		CAMERA = 4
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		~EditorLayer();

		void OnCreate() override;
		void OnDestroy() override;

		void Update(Timer timer) override;
		void RenderImGui() override;

		void ResetRenderer();

		void OnEvent(Event& e) override;

		void PathTracingView();
		
		void PathTracing();

	private:
		bool ShortCuts(KeyPressedEvent& e);

		void NewScene();
		void OpenScene(std::string& filepath);

		void SaveSceneAs();
		void SaveScene(const std::string& path);

		void SaveImage();

		void Restart();
	
	private:
		SceneHierarchyWindow hierarchy;
		ContentBrowserWindow contentBrowser;
		
		int guizmoState;
		
		std::shared_ptr<Scene> scene;

		std::shared_ptr<Framebuffer> sceneFramebuffer;
		std::shared_ptr<Framebuffer> viewportFramebuffer;
		std::shared_ptr<Framebuffer> accumulateFramebuffer;
		std::shared_ptr<Shader> lightingPass;
		std::shared_ptr<Shader> defaultShader;

		std::shared_ptr<TextureCube> skybox;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;
		
		std::shared_ptr<Texture2DArray> textureArray;

		std::shared_ptr<ShaderStorageBuffer> verticesSsbo;
		std::shared_ptr<ShaderStorageBuffer> indicesSsbo;
		std::shared_ptr<ShaderStorageBuffer> normalsSsbo;
		std::shared_ptr<ShaderStorageBuffer> objectsSsbo;
		std::shared_ptr<ShaderStorageBuffer> meshesSsbo;
		std::shared_ptr<ShaderStorageBuffer> transformsSsbo;
		std::shared_ptr<ShaderStorageBuffer> materialsSsbo;
		std::shared_ptr<ShaderStorageBuffer> aabbsSsbo;

		PerspectiveCamera camera;

		glm::vec2 viewSize;

		glm::vec2 rendererSize;

		// Rendering options
		int samples;
		int maxSamples;
		int maxBounces;

		NeedToUpdate needToUpdate;
		bool sceneChanged;

		bool imageSaved;
		bool startRenderer;
	};
}