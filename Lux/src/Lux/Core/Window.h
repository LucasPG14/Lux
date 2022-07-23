#pragma once

#include "luxpch.h"

#include "Core.h"
#include "Lux/Events/Event.h"

struct GLFWwindow;

namespace Lux
{
	class LUX_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window();
		virtual ~Window();

		void Update();

		unsigned int GetWidth() const { return data.width; }
		unsigned int GetHeight() const { return data.height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { data.eventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;

		GLFWwindow* GetWindow() const { return window; }

	private:
		void Init();
		void Shutdown();

	private:
		GLFWwindow* window;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool vSync;

			EventCallbackFn eventCallback;
		};

		WindowData data;
	};
}