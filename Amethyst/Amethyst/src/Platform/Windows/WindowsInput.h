#pragma once

#include "Amethyst/Core/Input.h"

namespace Amethyst
{
	class AMT_API WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(int keycode) override;

		bool IsMouseButtonPressedImpl(int button) override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;
	};
}