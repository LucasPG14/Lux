#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Amethyst
{
	class AMT_API Application
	{
	public:
		Application();

		~Application();

		void Run();

	private:
		std::unique_ptr<Window> window;
		bool running;
	};

	Application* CreateApp();
}