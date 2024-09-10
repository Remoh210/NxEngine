#include "GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<const NString&, ECS::Entity*>()
        (
            rttr::policy::ctor::as_raw_ptr,// should create an instance of the class as shared_ptr<ns_3d::node>
            rttr::default_arguments(nullptr)      // second argument is optional, so we provide the default value for it
        )   
        .property("Name", &GameObject::name)
        .property("Components", &GameObject::components)
    ;
}