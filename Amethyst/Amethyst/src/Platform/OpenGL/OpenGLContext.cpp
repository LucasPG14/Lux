#include "amtpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Amethyst
{
	OpenGLContext::OpenGLContext(GLFWwindow* glfwWindow) : window(glfwWindow)
	{
		AMT_CORE_ASSERT(window, "Window is NULL");
	}
	
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(window);

		// Initializing GLAD
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AMT_CORE_ASSERT(status, "Could not initialize OpenGL!");

		AMT_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VENDOR));
		AMT_CORE_INFO("OpenGL Version: {0}", glGetString(GL_RENDERER));
		AMT_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VERSION));
	}
	
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(window);
	}
}