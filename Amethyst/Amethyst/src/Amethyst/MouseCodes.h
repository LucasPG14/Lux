#pragma once
#include <stdint.h>

namespace Amethyst
{
	using KeyCode = uint16_t;

	namespace Mouse
	{
		enum : KeyCode
		{
			BUTTON_LEFT = 0,
			BUTTON_RIGHT = 1,
			BUTTON_MIDDLE = 2
		};
	}
}