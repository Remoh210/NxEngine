#pragma once

#include <dinput.h>
#include <rttr/registration>
#include "Core/ECS/ECS.h"

struct BaseComponent
{
    BaseComponent(ECS::Entity* entity)
        :m_entity(entity)
    {
        
    }
    
    virtual ~BaseComponent() = default;

    
    ECS::Entity* m_entity;
    
    RTTR_ENABLE()
};