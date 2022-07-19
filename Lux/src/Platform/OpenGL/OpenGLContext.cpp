#include "luxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Lux
{
	OpenGLContext::OpenGLContext(GLFWwindow* glfwWindow) : window(glfwWindow)
	{
		LUX_CORE_ASSERT(window, "Window is NULL");
	}
	
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(window);

		// Initializing GLAD
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LUX_CORE_ASSERT(status, "Could not initialize OpenGL!");

		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VENDOR));
		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_RENDERER));
		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VERSION));
	}
	
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(window);
	}
}