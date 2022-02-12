#include "amtpch.h"
#include "WindowsInput.h"

#include "Amethyst/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Amethyst
{
	Input* Input::input = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	
	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float WindowsInput::GetMouseXImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);
		
		return (float)posX;
	}
	
	float WindowsInput::GetMouseYImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);

		return (float)posY;
	}
}