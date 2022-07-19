#pragma once

#include "RenderOrder.h"
#include "Shader.h"
#include "Amethyst/Resources/Material.h"

#include "Amethyst/PerspectiveCamera.h"

namespace Amethyst
{
	class TransformComponent;
	class LightComponent;

	class Renderer
	{
	public:
		static void Init();

		static void BeginScene(const PerspectiveCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Material>& material, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& model = glm::mat4(1.0f));

		static void DrawQuad();

		static void ChangeState(bool change);

		inline static Render::API GetRenderer() { return Render::GetAPI(); }
		inline static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary() { return shaderLibrary; }

	private:
		struct SceneInfo
		{
			glm::mat4 viewProjection;
			glm::vec3 viewPos;
		};

		static std::unique_ptr<SceneInfo> sceneInfo;
		static std::unique_ptr<ShaderLibrary> shaderLibrary;
	};
}