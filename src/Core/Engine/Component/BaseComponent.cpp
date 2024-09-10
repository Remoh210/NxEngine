#include "BaseComponent.h"

#include "Core/ECS/ECS.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<BaseComponent>("BaseComponent")
        .constructor<>()
        (
            rttr::policy::ctor::as_object// should create an instance of the class as raw_ptr<>
        )   
    ;
}