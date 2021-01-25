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
	}

	void ECS::PhysicsSystem::tick(class World *world, float deltaTime)
	{
		physicsWorld->Update(deltaTime);

		world->each<TransformComponent, RigidBodyComponent>([&](Entity *ent, ComponentHandle<TransformComponent> transformComp ,ComponentHandle<RigidBodyComponent> rbComponent) -> void
		{
			transformComp->transform.position = rbComponent->rigidBody->GetPosition();
			vec3 rot = rbComponent->rigidBody->GetEulerRotation();
			transformComp->transform.rotation = vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
		});
	}

	ECS::PhysicsSystem::~PhysicsSystem()
	{
		physicsWorld->~iPhysicsWorld();
	}



}
