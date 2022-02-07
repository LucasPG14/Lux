#pragma once

#include "Amethyst/Core.h"

namespace Amethyst
{
	class AMT_API Input
	{
	public:
		static bool IsKeyPressed(int keycode) { return input->IsKeyPressedImpl(keycode); }
		static bool IsMouseButtonPressed(int button) { return input->IsMouseButtonPressedImpl(button); }
		static float GetMouseX() { return input->GetMouseXImpl(); }
		static float GetMouseY() { return input->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* input;
	};
}