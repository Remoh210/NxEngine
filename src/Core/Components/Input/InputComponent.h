#pragma once

#include "Common/Input/Input.h"
#include "Common/dataStructs/String.h"

struct InputComponent
{
	bool GetKeyPressed(InputKey KeyIn)
	{
		return KeyIn == pressedKey;
	}

	InputKey GetKeyPressed()
	{
		return pressedKey;
	}

	//"Horizontal" of "Vertical"
	float GetAxis(NString Axis)
	{
		return Axis == "Horizontal" ? horizontalAxis : verticalAxis;
	}

	void UpdateKeyPressed(InputKey keyIn)
	{
		 pressedKey = keyIn;
	}

	void UpdateAxis(float horizontal, float vertical)
	{
		horizontalAxis = horizontal;
		verticalAxis = vertical;
	}


private:
	InputKey pressedKey = InputKey::KEY_NONE;

	float horizontalAxis;
	float verticalAxis;
};