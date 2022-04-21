#pragma once

#include "Amethyst/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Amethyst
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* glfwWindow);

		void Init() override;
		void Shutdown() override;

		void SwapBuffers() override;
	private:
		GLFWwindow* window;
	};
}