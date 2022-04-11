#pragma once

#include "RenderOrder.h"
#include "Shader.h"
#include "Amethyst/Resources/Material.h"

#include "Amethyst/PerspectiveCamera.h"

namespace Amethyst
{
	class Renderer
	{
	public:
		static void Init();

		static void BeginScene(const PerspectiveCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material, const glm::mat4& model = glm::mat4(1.0f));

		inline static Render::API GetRenderer() { return Render::GetAPI(); }

	private:
		struct SceneInfo
		{
			glm::mat4 view;
			glm::mat4 projection;
		};

		static std::unique_ptr<SceneInfo> sceneInfo;
	};
}