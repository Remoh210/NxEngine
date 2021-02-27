#include "PhysicsSystem.h"

#include "Core/Components/Physics/RigidBodyComponent.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Physcis/BulletPhysics/cBulletPhysicsFactory.h"
#include "Core/Physcis/BulletPhysics/BulletDebugRenderer.h"

namespace ECS
{
	PhysicsSystem::PhysicsSystem()
	{
		debugDrawMode = 0;
		physicsFactory = new nPhysics::cBulletPhysicsFactory();
		physicsWorld = physicsFactory->CreatePhysicsWorld();
		debugRenderer = physicsWorld->GetDebugDrawer();
	}

	void ECS::PhysicsSystem::tick(class World *world, float deltaTime)
	{
		physicsWorld->Update(deltaTime);

		world->each<TransformComponent, RigidBodyComponent>([&](Entity *ent, ComponentHandle<TransformComponent> transformComp ,ComponentHandle<RigidBodyComponent> rbComponent) -> void
		{

			if (!rbComponent->bIsStatic)
			{
				transformComp->transform.position = rbComponent->rigidBody->GetPosition();
				vec3 rot = rbComponent->rigidBody->GetEulerRotation();
				transformComp->transform.rotation = rot;
			}

		});
	}

	ECS::PhysicsSystem::~PhysicsSystem()
	{
		physicsWorld->~iPhysicsWorld();
	}



}
