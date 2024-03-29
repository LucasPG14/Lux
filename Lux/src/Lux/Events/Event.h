#pragma once

#include "luxpch.h"
#include "Lux/Core/Core.h"

namespace Lux
{
	enum class EventType
	{
		NONE = 0,
		WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_DROP,
		KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
		MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED,
		MOUSE_MOVE, MOUSE_SCROLL
	};

	enum EventCategory
	{
		NONE = 0,
		EVENT_CATEGORY_APP,
		EVENT_CATEGORY_INPUT,
		EVENT_CATEGORY_KEYBOARD,
		EVENT_CATEGORY_MOUSE,
		EVENT_CATEGORY_MOUSE_BUTTON
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type;}\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class LUX_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	public:
		bool handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : e(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (e.GetEventType() == T::GetStaticType())
			{
				e.handled = func(*(T*)&e);
				return true;
			}
			return false;
		}
	private:
		Event& e;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}