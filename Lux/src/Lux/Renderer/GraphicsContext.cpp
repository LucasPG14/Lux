#include "luxpch.h"
#include "GraphicsContext.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Lux
{
	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetRenderer())
		{
		case Render::API::NONE: LUX_CORE_ASSERT(false, ""); return nullptr;
		case Render::API::OPENGL: return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
	}
}