#include "InputSystem.h"

InputKey ECS::InputSystem::GetKey()
{
	if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		return InputKey::KEY_SPACE;
	}
	if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_A) == GLFW_PRESS)
	{
		return InputKey::KEY_A;
	}
	if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_W) == GLFW_PRESS)
	{
		return InputKey::KEY_W;
	}
	if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_S) == GLFW_PRESS)
	{
		return InputKey::KEY_S;
	}
	if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_D) == GLFW_PRESS)
	{
		return InputKey::KEY_D;
	}
	else
	{
		return InputKey::KEY_NONE;
	}
}


