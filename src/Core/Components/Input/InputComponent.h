#pragma once

#include "Common/Input/Input.h"

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

	void UpdateKeyPressed(InputKey keyIn)
	{
		 pressedKey = keyIn;
	}
private:
	InputKey pressedKey = InputKey::KEY_NONE;
};