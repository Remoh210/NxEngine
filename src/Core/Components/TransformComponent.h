#pragma once
#include "Common/CommonTypes.h"
#include <Common/Transform/Transform.h>

struct TransformComponent
{
	TransformComponent(Transform TransformormIn)
		:transform(TransformormIn) {}

	TransformComponent() {}

	Transform transform;
};