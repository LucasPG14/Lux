#include "luxpch.h"
#include "RenderOrder.h"

#include "Platform/OpenGL/OpenGLRender.h"

namespace Lux
{
	Render* RenderOrder::render = new OpenGLRender;
}