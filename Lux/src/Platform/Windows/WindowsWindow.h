#pragma once

#include "Lux/Core/Window.h"
#include "Lux/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Lux 
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow();
		virtual ~WindowsWindow();

		void Update() override;

		inline unsigned int GetWidth() const override { return data.width; }
		inline unsigned int GetHeight() const override { return data.height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline void* GetWindow() const { return window; }
	private:
		virtual void Init();
		virtual void Shutdown();
	private:
		GLFWwindow* window;
		std::unique_ptr<GraphicsContext> context;

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