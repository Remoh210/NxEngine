#pragma once

#include <rttr/registration.h>

#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Physcis/iRigidBody.h"
#include "Common/Math/Math.h"
#include "Common/Transform/Transform.h"
#include "Core/Physcis/eBodyType.h"
#include "Core/Physcis/sRigidBodyDef.h"

struct shapeDef
{
	shapeDef()
	{
		halfExtents = vec3f(1.0f);
		planeNormal = vec3f(0.0f, 1.0f, 0.0f);
		radius = 1.0f;
		height = 1.0f;
		axis = 1;
		planeConstant = 0.0f;
	}
	
	vec3f halfExtents;
	vec3f planeNormal;
	float planeConstant;
	float radius;
	float height;
	int32 axis;
};

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
	void PostInitialize() override;

	bool bIsStatic = false;
	bool bIsPlayer = false;

	nPhysics::eShapeType shapeType = nPhysics::eShapeType::SHAPE_TYPE_SPHERE;
	
	vec3f offset;

	// Serialization for now is done without irb
	Transform transform;
	shapeDef shapeDef;
	float mass = 0.0f;

	
	nPhysics::iRigidBody* rigidBody;

private:
	nPhysics::sRigidBodyDef cachedRigidBodyDef;
	
	
	RTTR_ENABLE(BaseComponent)
};
