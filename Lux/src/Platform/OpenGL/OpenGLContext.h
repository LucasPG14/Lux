#pragma once

#include "Lux/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Lux
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* glfwWindow);

		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* window;
	};
}