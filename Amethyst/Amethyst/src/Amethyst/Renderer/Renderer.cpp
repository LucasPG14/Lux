 #include "amtpch.h"
#include "Renderer.h"

namespace Amethyst
{
	void Renderer::BeginScene()
	{
	}
	
	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		RenderOrder::Draw(vertexArray);
	}
}