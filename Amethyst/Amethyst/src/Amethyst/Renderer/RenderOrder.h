#pragma once

#include "Render.h"

namespace Amethyst
{
	class RenderOrder
	{
	public:
		inline static void Init()
		{
			render->Init();
		}

		inline static void ClearColor(const glm::vec4& color)
		{
			render->ClearColor(color);
		}
		
		inline static void Clear()
		{
			render->Clear();
		}

		inline static void Draw(const std::shared_ptr<VertexArray>& vertexArray)
		{
			render->Draw(vertexArray);
		}

	private:
		static Render* render;
	};
}