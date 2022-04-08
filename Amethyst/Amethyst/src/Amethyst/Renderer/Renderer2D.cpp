#include "amtpch.h"
#include "Renderer2D.h"

namespace Amethyst
{
	void Renderer2D::Init()
	{
		RenderOrder::Init();
	}

	void Renderer2D::BeginScene()
	{
	}
	
	void Renderer2D::EndScene()
	{
	}
	
	void Renderer2D::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		RenderOrder::Draw(vertexArray);
	}
}