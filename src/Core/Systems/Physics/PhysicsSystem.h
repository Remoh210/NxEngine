#pragma once 

#include "Core/ECS/ECS.h"
#include "Core/Physcis/iPhysicsFactory.h"
#include "Core/Physcis/iDebugRenderer.h"

class PhysicsSystem : public ECS::EntitySystem
{
public:
    PhysicsSystem();

    static void Initialize(ECS::World* ecsWorldIn);

    static PhysicsSystem* Get();

    virtual void tick(class ECS::World* world, float deltaTime);
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
    static PhysicsSystem* singletonSystemInstance;
    static ECS::World* ecsWorld;
    static nPhysics::iPhysicsFactory* physicsFactory;
    static nPhysics::iPhysicsWorld* physicsWorld;

    static nPhysics::iDebugRenderer* debugRenderer;

    static bool debugDrawMode;
};