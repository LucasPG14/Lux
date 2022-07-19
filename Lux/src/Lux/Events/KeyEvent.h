#pragma once

#include "Event.h"

namespace Lux 
{
	class LUX_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return keyCode; }

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)
	protected:
		KeyEvent(int keycode)
			: keyCode(keycode) {}

		int keyCode;
	};

	class LUX_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << keyCode << " (" << repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_PRESSED)
	private:
		int repeatCount;
	};

	class LUX_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_RELEASED)
	};

	class LUX_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_TYPED)
	};
}