#include "PhysicsSystem.h"

#include "Core/Components/Physics/RigidBodyComponent.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Physcis/BulletPhysics/cBulletPhysicsFactory.h"
#include "Core/Application/Application.h"
#include "Core/Application/SceneManager/GameObject.h"

PhysicsSystem* PhysicsSystem::singletonSystemInstance = nullptr;
ECS::World* PhysicsSystem::ecsWorld = nullptr;
nPhysics::iPhysicsFactory* PhysicsSystem::physicsFactory= nullptr;
nPhysics::iPhysicsWorld* PhysicsSystem::physicsWorld= nullptr;
nPhysics::iDebugRenderer* PhysicsSystem::debugRenderer= nullptr;
bool PhysicsSystem::debugDrawMode = 0;

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::Initialize(ECS::World* ecsWorldIn)
{
    debugDrawMode = 0;
    physicsFactory = new nPhysics::cBulletPhysicsFactory();
    physicsWorld = physicsFactory->CreatePhysicsWorld();
    physicsWorld->SetGravity(glm::vec3(0.0f, -600.0f, 0.0f));
    debugRenderer = physicsWorld->GetDebugDrawer();
    ecsWorld = ecsWorldIn;
}

PhysicsSystem* PhysicsSystem::Get()
{
    if(!singletonSystemInstance)
    {
        if(!ecsWorld)
        {
            DEBUG_LOG_TEMP("ECS world is invalid");
            return nullptr;
        }
        singletonSystemInstance = new PhysicsSystem();
        ecsWorld->registerSystem(singletonSystemInstance);
    }

    return singletonSystemInstance;
}

void PhysicsSystem::tick(class ECS::World *world, float deltaTime)
{
    physicsWorld->DrawDebug();

    if (!Application::GetIsPIE())
    {
        return;
    }
    physicsWorld->Simulate(deltaTime);

    world->each<TransformComponent, RigidBodyComponent>([&](ECS::Entity *ent, ECS::ComponentHandle<TransformComponent> transformComp, ECS::ComponentHandle<RigidBodyComponent> rbComponent) -> void
    {
        if (!rbComponent->bIsStatic)
        {
            transformComp->transform.position = rbComponent->rigidBody->GetPosition() + rbComponent->offset.ToVec();
            transformComp->transform.rotation = rbComponent->rigidBody->GetEulerRotation();
        }
    });
}

PhysicsSystem::~PhysicsSystem()
{
    physicsWorld->~iPhysicsWorld();
}