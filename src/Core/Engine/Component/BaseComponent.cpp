#include "BaseComponent.h"

#include "Core/ECS/ECS.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<BaseComponent>("BaseComponent")
        .constructor<>()
        (
            rttr::policy::ctor::as_raw_ptr
        )   
    ;
}

void BaseComponent::Initialize(ECS::Entity* entityIn)
{
    if(entityIn)
    {
        entity = entityIn;
    }
}
