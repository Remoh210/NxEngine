#pragma once

#include <dinput.h>
#include <rttr/registration>
#include "Core/ECS/ECS.h"

struct BaseComponent
{
    BaseComponent()
    {
        m_entity = nullptr;
    }
    
    BaseComponent(ECS::Entity* entity)
        :m_entity(entity)
    {
        
    }
    
    virtual ~BaseComponent() = default;

    
    ECS::Entity* m_entity;
    
    RTTR_ENABLE()
};