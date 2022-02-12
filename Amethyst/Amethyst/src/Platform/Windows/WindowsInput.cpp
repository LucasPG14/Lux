#include "amtpch.h"
#include "Amethyst/Core/Input.h"

#include "Amethyst/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Amethyst
{
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	
	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float Input::GetMouseX()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);
		
		return (float)posX;
	}
	
	float Input::GetMouseY()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);

		return (float)posY;
	}
}