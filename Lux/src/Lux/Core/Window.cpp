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