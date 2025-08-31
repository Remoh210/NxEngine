#pragma once
#include "Common/Math/Math.h"
#include "Core/Engine/Component/BaseComponent.h"

#include <rttr/registration>

struct CharacterComponent : public BaseComponent
{
	CharacterComponent()
	{
		
	}
	
	CharacterComponent(ECS::Entity* entity)
		:BaseComponent(entity)
	{
		
	}
	CharacterComponent(ECS::Entity* entity, float movementSpeedIn)
		:BaseComponent(entity)
		,movementSpeed(movementSpeedIn)
	{
		
	}

	float movementSpeed = 10.0f;
	vec3f cameraRelativePosition = vec3f(0.0f);

	void Initialize(ECS::Entity* entityIn) override;
	
	RTTR_ENABLE(BaseComponent)
};