#include "amtpch.h"
#include "RenderOrder.h"

#include "Platform/OpenGL/OpenGLRender.h"

namespace Amethyst
{
	Render* RenderOrder::render = new OpenGLRender;
}