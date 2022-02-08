#pragma once

#include "Amethyst/Renderer/Render.h"

namespace Amethyst
{
	class OpenGLRender : public Render
	{
	public:
		void ClearColor(const glm::vec4& color) const override;
		void Clear() const override;

		void Draw(const std::shared_ptr<VertexArray>& vertexArray) override;
		
	};
}