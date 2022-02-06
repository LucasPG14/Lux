#include "amtpch.h"
#include "Application.h"

namespace Amethyst
{
	Application::Application() : running(true)
	{
		window = std::unique_ptr<Window>(Window::Create());
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		while (running)
		{
			window->Update();
		}
	}
}