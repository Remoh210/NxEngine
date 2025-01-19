#include "GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<>()
        (
            rttr::policy::ctor::as_raw_ptr// should create an instance of the class as shared_ptr<ns_3d::node>
        )   
        // .constructor<const NString&, ECS::Entity*>()
        // (
        //     rttr::policy::ctor::as_raw_ptr,// should create an instance of the class as shared_ptr<ns_3d::node>
        //     rttr::default_arguments(nullptr)      // second argument is optional, so we provide the default value for it
        // )   
        .property("Name", &GameObject::name)
        .property("Components", &GameObject::components)
    ;
}

void GameObject::Initialize(ECS::World* worldIn, const NString& name)
{
    if(!worldIn)
    {
        DEBUG_LOG_TEMP("Unable to init game object, world is invalid");
    }
    world = worldIn;
    entity = world->create();
    
    for(BaseComponent* component : components)
    {
        if(component)
        {
            component->GameObject = this;
            component->Initialize(entity);
        }
    }
    
    for(BaseComponent* component : components)
    {
        if(component)
        {
            component->PostInitialize();
        }
    }
}
