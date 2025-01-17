#pragma once

#include <dinput.h>
#include <rttr/registration>
#include "Core/ECS/ECS.h"

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
    
    virtual ~BaseComponent() = default;
    
    ECS::Entity* entity = nullptr;

    //TODO Copy constructor?
    
    RTTR_ENABLE()
};