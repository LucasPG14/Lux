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

	private:
		bool ShortCuts(KeyPressedEvent& e);
	
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
		std::shared_ptr<Shader> skyboxShader;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;

		std::shared_ptr<Shader> outputShader;
		std::shared_ptr<ComputeShader> computeShader;

		std::shared_ptr<Texture2D> transformsTexture;
		std::shared_ptr<Texture2D> aabbsTexture;
		std::shared_ptr<Texture2D> objectsTexture;
		
		std::shared_ptr<Texture2DArray> textureArray;

		PerspectiveCamera camera;

		glm::vec2 viewSize;

		// Rendering options
		int samples;
		int maxSamples;

		NeedToUpdate needToUpdate;
		bool sceneChanged;
	};
}