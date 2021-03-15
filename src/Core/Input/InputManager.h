#pragma once

#include "Common/Input/InputTypes.h"
#include "Common/dataStructs/String.h"

#include "rendering/Window.h"


class InputManager
{
public:
	static void Initialize(Window* windowIn);

	static bool GetKeyPressed(InputKey keyIn);
	static float GetAxis(NString axis);

private:
	static Window* window;
};