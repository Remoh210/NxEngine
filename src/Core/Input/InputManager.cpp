#include "InputManager.h"

Window* InputManager::window = nullptr;

void InputManager::Initialize(Window* windowIn)
{
	window = windowIn;
}

bool InputManager::GetKeyPressed(InputKey keyIn)
{
	return window->GetKeyPressed(keyIn);
}

float InputManager::GetAxis(NString axis)
{
	if (axis == "Horizontal")
		return window->GetHorizontalInputAxis();
	else if (axis == "Vertical")
		return window->GetVerticalInputAxis();
	else
		return 0.0f;
}
