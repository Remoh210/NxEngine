#pragma once
#include "Common/CommonTypes.h"
#include <Common/Transform/Transform.h>
#include "rttr/registration"

struct TransformComponent
{
	TransformComponent(Transform TransformormIn)
		:transform(TransformormIn) {}

	TransformComponent() {}

	Transform transform;

	float rotSpeed = 0.0f;

	RTTR_ENABLE();
};

RTTR_REGISTRATION
{
	rttr::registration::class_<TransformComponent>("TransformComponent")
		.property("transform", &TransformComponent::transform)
		.property("Rotation Speed", &TransformComponent::rotSpeed)
	;
}