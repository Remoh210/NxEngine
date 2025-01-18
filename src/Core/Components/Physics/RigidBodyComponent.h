#pragma once

#include <rttr/registration.h>

#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Physcis/iRigidBody.h"
#include "Common/Math/Math.h"
#include "Common/Transform/Transform.h"

struct RigidBodyComponent : public BaseComponent
{
	RigidBodyComponent()
	{
		
	}
	
	RigidBodyComponent(ECS::Entity* entity, nPhysics::iRigidBody* rigidBodyIn, vec3f offsetIn)
		:BaseComponent(entity)
		,rigidBody(rigidBodyIn)
		,offset(offsetIn)
	{
	}
	RigidBodyComponent(ECS::Entity* entity, nPhysics::iRigidBody* rigidBodyIn)
		:BaseComponent(entity)
		,rigidBody(rigidBodyIn)
	{
		offset = vec3f(0.f);
	}
	RigidBodyComponent(ECS::Entity* entity)
		:BaseComponent(entity)
	{
		rigidBody = nullptr;
		offset = vec3f(0.f);
	}

	void Initialize(ECS::Entity* entityIn) override;

	bool bIsStatic = false;
	nPhysics::iRigidBody* rigidBody;
	vec3f offset;

	// Serialization for now is done without irb
	Transform transform;
	float mass = 0.0f;


	RTTR_ENABLE(BaseComponent)
};
