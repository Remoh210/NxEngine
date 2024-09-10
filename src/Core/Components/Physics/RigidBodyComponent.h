#pragma once
#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Physcis/iRigidBody.h"
#include "Common/Math/Math.h"

struct RigidBodyComponent : public BaseComponent
{
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

	bool bIsStatic = false;
	nPhysics::iRigidBody* rigidBody;
	vec3f offset;
	//bool bSimulatePhysics;
};