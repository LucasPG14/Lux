#include "amtpch.h"
#include "WindowsWindow.h"

namespace Amethyst 
{
	Window* Window::Create()
	{
		return new WindowsWindow();
	}

	WindowsWindow::WindowsWindow()
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		data.title = "Amethyst Editor";
		data.width = 1280;
		data.height = 720;

		AMT_CORE_INFO("Creating window {0} ({1}, {2})", data.title, data.width, data.height);

		int success = glfwInit();
		AMT_CORE_ASSERT(success, "Could not intialize GLFW!");

		window = glfwCreateWindow((int)data.width, (int)data.height, data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, &data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(window);
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		data.vSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return data.vSync;
	}

}