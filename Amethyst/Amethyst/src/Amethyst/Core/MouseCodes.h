#pragma once
#include <stdint.h>

namespace Amethyst
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			BUTTON_LEFT = 0,
			BUTTON_RIGHT = 1,
			BUTTON_MIDDLE = 2
		};
	}
}