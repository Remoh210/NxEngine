#pragma once

#include "Core/Engine/Component/BaseComponent.h"
#include "Common/Transform/Transform.h"

#include <rttr/registration>

struct TransformComponent : public BaseComponent
{
	TransformComponent()
	{

	}
	
	TransformComponent(ECS::Entity* entity, const Transform& transformormIn)
		:BaseComponent(entity),
		transform(transformormIn)
	{

	}
	
	Transform transform;
	float rotSpeed = 0.0f;
	
	virtual void Initialize(ECS::Entity* entityIn) override;

	RTTR_ENABLE(BaseComponent)
};