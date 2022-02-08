#pragma once

#include "RenderOrder.h"

namespace Amethyst
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static Render::API GetRenderer() { return Render::GetAPI(); }
	};
}