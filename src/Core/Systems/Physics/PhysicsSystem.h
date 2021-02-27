#pragma once 

#include "Core/ECS/ECS.h"
#include "Core/Physcis/iPhysicsFactory.h"
#include "Core/Physcis/iDebugRenderer.h"

namespace ECS
{
	class PhysicsSystem : public EntitySystem
	{
	public:
		PhysicsSystem();

		virtual void tick(class World *world, float deltaTime);
		~PhysicsSystem();

		inline nPhysics::iPhysicsWorld* GetWorld()
		{
			return physicsWorld;
		}

		inline nPhysics::iPhysicsFactory* GetFactory()
		{
			return physicsFactory;
		}

		inline void ToggleDebugDraw()
		{
			debugDrawMode = !debugDrawMode;
			physicsWorld->SetDebugDrawerMode(static_cast<int>(debugDrawMode));
		}

	private:
		nPhysics::iPhysicsFactory* physicsFactory;
		nPhysics::iPhysicsWorld* physicsWorld;

		nPhysics::iDebugRenderer* debugRenderer;

		bool debugDrawMode;
	};
}
