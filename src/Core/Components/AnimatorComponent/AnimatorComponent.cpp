#include "AnimatorComponent.h"
#include "Core/Application/AssetManager/AssetManager.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<AnimatorComponent>("AnimatorComponent")
        .constructor<>()
    (
        rttr::policy::ctor::as_raw_ptr
    )
        .property("Animations", &AnimatorComponent::animNameToFileMap)
        .property("Animation States", &AnimatorComponent::animationStates)
        .property("Initial State", &AnimatorComponent::initialState)
    ;
}

void AnimatorComponent::Initialize(ECS::Entity* entityIn)
{
    BaseComponent::Initialize(entityIn);

    for(const auto& it : animNameToFileMap)
    {
        const NString& animName = it.first;
        const NString& animFile = it.second;

        if(!animName.empty() && !animFile.empty())
        {
            AnimationInfo* NewAnimInfo = AssetManager::ImportAnimation(animFile, animName);

            if(NewAnimInfo)
            {
                animationMap.emplace(animName, NewAnimInfo);
            }
        }
    }


    // no support for raw models for now
    if(!animationMap.empty())
    {
        entity->assign<AnimatorComponent>(*this);
    }
}
