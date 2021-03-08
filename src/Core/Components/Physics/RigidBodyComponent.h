#pragma once
#include "Core/Physcis/iRigidBody.h"
#include "Common/Math/Math.h"

struct RigidBodyComponent
{
	RigidBodyComponent(nPhysics::iRigidBody* rigidBodyIn, vec3f offsetIn)
		:rigidBody(rigidBodyIn)
		,offset(offsetIn)
	{
	}
	RigidBodyComponent(nPhysics::iRigidBody* rigidBodyIn)
		:rigidBody(rigidBodyIn)
	{
		offset = vec3f(0.f);
	}
	RigidBodyComponent()
	{
		rigidBody = nullptr;
		offset = vec3f(0.f);
	}

	bool bIsStatic = false;
	nPhysics::iRigidBody* rigidBody;
	vec3f offset;
	//bool bSimulatePhysics;
};