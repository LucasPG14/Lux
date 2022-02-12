#pragma once

#include "Amethyst/Core/Core.h"
#include "Amethyst/Core/KeyCodes.h"
#include "Amethyst/Core/MouseCodes.h"

namespace Amethyst
{
	class AMT_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);
		static float GetMouseX();
		static float GetMouseY();
	};
}