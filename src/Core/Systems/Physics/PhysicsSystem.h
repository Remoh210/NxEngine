#pragma once 

#include "Core/ECS/ECS.h"
#include "Core/Physcis/iPhysicsFactory.h"
#include "Core/Physcis/BulletPhysics/cBulletPhysicsFactory.h"

class PhysicsSystem
{
public:
	PhysicsSystem()
	{
		physicsFactory = new cBulletPhysicsFactory();
		physicsWorld = physicsFactory->CreatePhysicsWorld();
	}
	~PhysicsSystem();

private:
	iPhysicsFactory* physicsFactory;
	iPhysicsWorld* physicsWorld;
};

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}