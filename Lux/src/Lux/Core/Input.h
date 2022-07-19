#pragma once

#include "Lux/Core/Core.h"
#include "Lux/Core/KeyCodes.h"
#include "Lux/Core/MouseCodes.h"

namespace Lux
{
	class LUX_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);
		static float GetMouseX();
		static float GetMouseY();
	};
}