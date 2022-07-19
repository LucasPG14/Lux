#pragma once

#include "Lux/Renderer/Render.h"

namespace Lux
{
	class OpenGLRender : public Render
	{
	public:
		void Init() const override;
		void ClearColor(const glm::vec4& color) const override;
		void Clear() const override;

		void ChangeState(bool change) const override;

		void Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		
	};
}