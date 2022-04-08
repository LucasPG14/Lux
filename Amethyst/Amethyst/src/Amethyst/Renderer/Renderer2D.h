#pragma once

#include "RenderOrder.h"

namespace Amethyst
{
	class Renderer2D
	{
	public:
		static void Init();

		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static Render::API GetRenderer() { return Render::GetAPI(); }
	};
}