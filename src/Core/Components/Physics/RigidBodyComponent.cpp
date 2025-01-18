#include "RigidBodyComponent.h"

#include "Core/Application/SceneManager/GameObject.h"
#include "core/Physcis/iPhysicsFactory.h"
#include "Core/Systems/Physics/PhysicsSystem.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RigidBodyComponent>("RigidBodyComponent")
    .constructor<>()
    (
        rttr::policy::ctor::as_raw_ptr
    )
        .property("Is Static", &RigidBodyComponent::bIsStatic)
        .property("Transform", &RigidBodyComponent::transform)
        .property("Mass", &RigidBodyComponent::mass)
        .property("Offset", &RigidBodyComponent::offset)
    ;
}

void RigidBodyComponent::Initialize(ECS::Entity* entityIn)
{
    BaseComponent::Initialize(entityIn);

    ECS::World* world = GameObject ? GameObject->world : nullptr;

    if(!world)
    {
        DEBUG_LOG_TEMP("RigidBodyComponent::Initialize: world is null!");
        return;
    }

    nPhysics::sRigidBodyDef newRigidBodyDef;
    newRigidBodyDef.Orientation = vec3(
        glm::radians(transform.rotation[0]),
        glm::radians(transform.rotation[1]),
        glm::radians(transform.rotation[2]));
    newRigidBodyDef.Position = transform.position.ToVec();
    newRigidBodyDef.Scale = transform.scale.ToVec();

    newRigidBodyDef.Mass = 0.0f;
    newRigidBodyDef.GameObjectName = GameObject ? GameObject->name : "None";
    nPhysics::iShape* newShape = PhysicsSystem::Get()->GetFactory()->CreateSphereShape(10.f);
    rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(newRigidBodyDef, newShape);
    PhysicsSystem::Get()->GetWorld()->AddBody(rigidBody);
}
