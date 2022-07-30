#include "luxpch.h"
#include "Window.h"

#include "Lux/Events/ApplicationEvent.h"
#include "Lux/Events/MouseEvent.h"
#include "Lux/Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Lux
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		LUX_CORE_ERROR("GLFW error ({0}): {1}", error, description);
	}

	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::string messageError = "Debug message ( + " + std::to_string(id) + " ): " + std::string(message);

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             messageError += "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   messageError += "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: messageError += "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     messageError += "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     messageError += "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           messageError += "Source: Other"; break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               messageError += "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: messageError += "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  messageError += "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         messageError += "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         messageError += "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              messageError += "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          messageError += "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           messageError += "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               messageError += "Type: Other"; break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         LUX_CORE_ERROR(messageError); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       LUX_CORE_WARN(messageError); break;
		case GL_DEBUG_SEVERITY_LOW:          LUX_CORE_INFO(messageError); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LUX_CORE_TRACE(messageError); break;
		}
	}

	Window::Window()
	{
		Init();
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::Init()
	{
		// Window default information
		data.title = "Lux Editor";
		data.width = 1280;
		data.height = 720;

		LUX_CORE_INFO("Creating window {0} ({1}, {2})", data.title, data.width, data.height);

#ifdef LUX_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

		// Initializing GLFW
		int success = glfwInit();
		LUX_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);

		// Creating GLFW window
		window = glfwCreateWindow((int)data.width, (int)data.height, data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(window);


		// Initializing OpenGL with GLAD
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LUX_CORE_ASSERT(status, "Could not initialize OpenGL!");

		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VENDOR));
		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_RENDERER));
		LUX_CORE_INFO("OpenGL Version: {0}", glGetString(GL_VERSION));

#ifdef LUX_DEBUG
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif

		// Setting GLFW User Pointer
		glfwSetWindowUserPointer(window, &data);
		SetVSync(true);

		// Setting Window Resize Callback
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		// Setting Window Close Callback
		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.eventCallback(event);
		});

		// Setting Key Callback
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.eventCallback(event);
				break;
			}
			}
		});

		// Setting Char Callback
		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int key)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(key);
			data.eventCallback(event);
		});

		// Setting Mouse Button Callback
		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
			}
		});

		// Setting Mouse Scroll Callback
		glfwSetScrollCallback(window, [](GLFWwindow* window, double offsetX, double offsetY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)offsetX, (float)offsetY);
			data.eventCallback(event);
		});

		// Setting Mouse Position Callback
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX, double posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)posX, (float)posY);
			data.eventCallback(event);
		});

		glfwSetDropCallback(window, [](GLFWwindow* window, int size, const char* paths[])
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			std::vector<std::string> filePaths;
			for (int i = 0; i < size; ++i)
				filePaths.push_back(paths[i]);

			WindowDropEvent event(filePaths);
			data.eventCallback(event);
		});
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(window);
	}

	void Window::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		data.vSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return data.vSync;
	}
}