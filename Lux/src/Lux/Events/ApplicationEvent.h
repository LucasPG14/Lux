#pragma once

#include "Event.h"

namespace Lux 
{
	class LUX_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int w, unsigned int h)
			: width(w), height(h) {}

		inline unsigned int GetWidth() const { return width; }
		inline unsigned int GetHeight() const { return height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_RESIZE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APP)
	private:
		unsigned int width, height;
	};

	class LUX_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WINDOW_CLOSE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APP)
	};

	class LUX_API WindowDropEvent : public Event
	{
	public:
		WindowDropEvent(std::vector<std::string>& filePaths) : paths(filePaths){}

		inline std::vector<std::string>& GetPaths() { return paths; }

		EVENT_CLASS_TYPE(WINDOW_DROP)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APP)
	private:
		std::vector<std::string> paths;
	};
}