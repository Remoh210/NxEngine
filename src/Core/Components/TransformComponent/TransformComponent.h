#pragma once

#include "Core/Engine/Component/BaseComponent.h"
#include "Common/Transform/Transform.h"

#include <rttr/registration>

struct TransformComponent : public BaseComponent
{
	TransformComponent()
	{

	}
	
	TransformComponent(ECS::Entity* entity)
		:BaseComponent(entity)
	{
		Initialize();
	}
	
	TransformComponent(ECS::Entity* entity, const Transform& transformormIn)
		:BaseComponent(entity),
		transform(transformormIn)
	{
		Initialize();
	}

	Transform transform;

	float rotSpeed = 0.0f;
	
	RTTR_ENABLE()
	
private:
	void Initialize();
};