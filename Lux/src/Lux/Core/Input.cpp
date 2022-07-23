#include "luxpch.h"
#include "Lux/Core/Input.h"

#include "Lux/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Lux
{
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = Application::Get().GetWindow().GetWindow();
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	
	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = Application::Get().GetWindow().GetWindow();
		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float Input::GetMouseX()
	{
		auto window = Application::Get().GetWindow().GetWindow();
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);
		
		return (float)posX;
	}
	
	float Input::GetMouseY()
	{
		auto window = Application::Get().GetWindow().GetWindow();
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);

		return (float)posY;
	}
}