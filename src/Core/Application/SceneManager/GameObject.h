#pragma once

#include "Common/dataStructs/String.h"
#include "Core/ECS/ECS.h"
#include "Core/Engine/Component/BaseComponent.h"
#include <rttr/registration>
#include <cereal/types/memory.hpp>    // for std::shared_ptr
#include <cereal/types/vector.hpp>    // for std::vector

class GameObject
{
public:
     GameObject()
         :entity(nullptr)
         ,name("None")
     {}
     
     GameObject(const NString& InObjectName, ECS::Entity* InEntity)
         :entity(InEntity)
         ,name(InObjectName)
     {
         if(InEntity)
         {
             NxArray<ECS::ComponentHandle<BaseComponent>> componentHandles = InEntity->getAllComponents<BaseComponent>();
             for (ECS::ComponentHandle<BaseComponent>& component : componentHandles)
             {
                 if(!component)
                 {
                     continue;
                 }
	   
                 components.push_back((BaseComponent*)&component.get());
             }
         }
     }
    
     
    ECS::Entity* entity;
    NxArray<BaseComponent*> components;
    NString name;
    
    RTTR_ENABLE()
};