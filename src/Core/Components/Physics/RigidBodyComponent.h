#pragma once
#include "Core/Physcis/iRigidBody.h"

struct RigidBodyComponent
{
	RigidBodyComponent(nPhysics::iRigidBody* rigidBodyIn)
		:rigidBody(rigidBodyIn)
	{
	}
	RigidBodyComponent()
	{
	}

	bool bIsStatic = false;
	nPhysics::iRigidBody* rigidBody;
	//bool bSimulatePhysics;
};