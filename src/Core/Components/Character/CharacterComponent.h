#pragma once
#include "Common/Math/Math.h"
#include "rttr/registration"

struct CharacterComponent
{
	CharacterComponent() {}
	CharacterComponent(float movementSpeedIn);

	float movementSpeed = 10.0f;
	vec3f cameraRelativePosition = vec3f(0.0f);

private:
	RTTR_ENABLE();
};