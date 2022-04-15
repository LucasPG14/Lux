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
		inline static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary() { return shaderLibrary; }

		struct Statistics
		{
			uint32_t drawCalls;
			uint32_t trianglesCount;

			uint32_t GetTotalDrawCalls() { return drawCalls; }
			uint32_t GetTotalTriangles() { return trianglesCount; }
		};

	private:
		static void StartBatch();
		static void Flush();
		static void NextBatch();

	private:
		struct SceneInfo
		{
			glm::mat4 viewProjection;
		};

		static std::unique_ptr<SceneInfo> sceneInfo;
		static std::unique_ptr<ShaderLibrary> shaderLibrary;
	};
}