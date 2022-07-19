#pragma once

#include "luxpch.h"

#include "Core.h"
#include "Lux/Events/Event.h"

namespace Lux
{
	class LUX_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void Update() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetWindow() const = 0;

		static Window* Create();
	};
}