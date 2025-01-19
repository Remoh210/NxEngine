#pragma once

#include <dinput.h>
#include <rttr/registration>
#include "Core/ECS/ECS.h"

class GameObject;

struct BaseComponent
{
    BaseComponent()
    {
        
    }
    
    BaseComponent(ECS::Entity* entity)
        :entity(entity)
    {
        
    }
    
    virtual void Initialize(ECS::Entity* entityIn);

    // Called after all componets are initialized
    virtual void PostInitialize() {}
    
    virtual ~BaseComponent() = default;
    
    ECS::Entity* entity = nullptr;

    GameObject* GameObject = nullptr;

    //TODO Copy constructor?
    
    RTTR_ENABLE()
};