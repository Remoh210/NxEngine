#include "RigidBodyComponent.h"

#include "Core/Application/AssetManager/AssetManager.h"
#include "Core/Application/SceneManager/GameObject.h"
#include "Core/Components/StaticMeshComponent/StaticMeshComponent.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "core/Physcis/iPhysicsFactory.h"
#include "Core/Systems/Physics/PhysicsSystem.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<shapeDef>("shapeDef")
        .constructor<>()
    (
        rttr::policy::ctor::as_object
    )
        .property("Half Extents", &shapeDef::halfExtents)
        .property("Radius", &shapeDef::radius)
        .property("Height", &shapeDef::height)
        .property("Axis", &shapeDef::axis)
        .property("planeNormal", &shapeDef::planeNormal)
        .property("planeConstant", &shapeDef::planeConstant)
    ;
    
    rttr::registration::class_<RigidBodyComponent>("RigidBodyComponent")
    .constructor<>()
    (
        rttr::policy::ctor::as_raw_ptr
    )
        .property("Is Static", &RigidBodyComponent::bIsStatic)
        .property("Shape Def", &RigidBodyComponent::shapeDef)
        .property("Mass", &RigidBodyComponent::mass)
        .property("Offset", &RigidBodyComponent::offset)
        .property("Shape Type", &RigidBodyComponent::shapeType)
    ;

    rttr::registration::enumeration<nPhysics::eShapeType>("ShapeType")
    (
        rttr::value("Box", nPhysics::eShapeType::SHAPE_TYPE_BOX),
        rttr::value("Mesh", nPhysics::eShapeType::SHAPE_TYPE_MESH),
        rttr::value("Plane", nPhysics::eShapeType::SHAPE_TYPE_PLANE),
        rttr::value("Sphere", nPhysics::eShapeType::SHAPE_TYPE_SPHERE),
        rttr::value("Capsule", nPhysics::eShapeType::SHAPE_TYPE_CAPSULE),
        rttr::value("Cylinder", nPhysics::eShapeType::SHAPE_TYPE_CYLINDER),
        rttr::value("Convex", nPhysics::eShapeType::SHAPE_TYPE_CONVEX_HULL)
    );
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

    std::vector<ECS::ComponentHandle<TransformComponent>> componentHandles = entity->getAllComponents<TransformComponent>();
    if(componentHandles.size() > 0)
    {
        transform = componentHandles[0].get().transform;
    }
    
    cachedRigidBodyDef.Orientation = vec3(
        glm::radians(transform.rotation[0]),
        glm::radians(transform.rotation[1]),
        glm::radians(transform.rotation[2]));
    cachedRigidBodyDef.Position = transform.position.ToVec();
    cachedRigidBodyDef.Scale = transform.scale.ToVec();

    cachedRigidBodyDef.Mass = mass;
    cachedRigidBodyDef.GameObjectName = GameObject ? GameObject->name : "None";
    
    nPhysics::iShape* newShape = nullptr;
    switch (shapeType)
    {
    case nPhysics::SHAPE_TYPE_BOX:
        {
            newShape  = PhysicsSystem::Get()->GetFactory()->CreateBoxShape(shapeDef.halfExtents.ToVec());
            break;
        }
    case nPhysics::SHAPE_TYPE_SPHERE:
        {
            newShape  = PhysicsSystem::Get()->GetFactory()->CreateSphereShape(shapeDef.radius);
            break;
        }
    case nPhysics::SHAPE_TYPE_MESH:
        {
            // Do nothing here
            break;
        }
    case nPhysics::SHAPE_TYPE_CAPSULE:
        {
            newShape  = PhysicsSystem::Get()->GetFactory()->CreateCapsuleShape(shapeDef.height, shapeDef.radius, shapeDef.axis);
            break;
        }
    case nPhysics::SHAPE_TYPE_PLANE:
        {
            newShape  = PhysicsSystem::Get()->GetFactory()->CreatePlaneShape(shapeDef.planeNormal.ToVec(), shapeDef.planeConstant);
            break;
        } 
    default:
        {
            break;
        }
    }

    if(!newShape)
    {
        DEBUG_LOG_TEMP("Shape is invalid!");
        return;
    }
    
    rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(cachedRigidBodyDef, newShape);
    PhysicsSystem::Get()->GetWorld()->AddBody(rigidBody);


    entityIn->assign<RigidBodyComponent>(*this);
}

void RigidBodyComponent::PostInitialize()
{
    BaseComponent::PostInitialize();

    if(rigidBody)
    {
        return;
    }

    if(shapeType != nPhysics::SHAPE_TYPE_MESH)
    {
        return;
    }

    nPhysics::iShape* newShape = nullptr;
    std::vector<ECS::ComponentHandle<StaticMeshComponent>> componentHandles = entity->getAllComponents<StaticMeshComponent>();
    if(componentHandles.size() > 0)
    {
        NxArray<IndexedModel> indexedModel;
        StaticMeshComponent& staticMeshComp = componentHandles[0].get();

        for(const MeshInfo* meshInfo : staticMeshComp.meshes)
        {
            if(meshInfo && meshInfo->vertexArray)
            {
                indexedModel.push_back(meshInfo->vertexArray->GetIndexedModel());
            }
        }
        
        if(indexedModel.size() > 0)
        {
            newShape = AssetManager::GenerateCollisionShapeFromModels(indexedModel);
        }
    }
   

    rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(cachedRigidBodyDef, newShape);
    PhysicsSystem::Get()->GetWorld()->AddBody(rigidBody);
}
