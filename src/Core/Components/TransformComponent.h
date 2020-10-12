#pragma once
#include "Common/CommonTypes.h"
#include <Common/Transform/Transform.h>
#include "Core/ECS/ECS.h"

struct TransformComponent : public Component<TransformComponent>
{
	Transform transform;
};