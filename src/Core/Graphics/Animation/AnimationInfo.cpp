#include "AnimationInfo.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<AnimTransition>("Animation Transition")
        .constructor<>()
    (
        rttr::policy::ctor::as_object
    )
        .property("Key Press", &AnimTransition::conditionKey)
    ;

    rttr::registration::class_<StateDetails>("State Details")
    .constructor<>()
    (
        rttr::policy::ctor::as_object
    )
    .property("Name", &StateDetails::name)
    .property("Has Exit Time", &StateDetails::bHasExitTime)
    ;

    rttr::registration::class_<AnimationState>("Animation State")
    .constructor<>()
    (
        rttr::policy::ctor::as_object
    )
    .property("Active Animation", &AnimationState::activeAnimation)
    .property("Previous", &AnimationState::prevAnimation)
    .property("Next", &AnimationState::nextAnimation)
    .property("Transition Map", &AnimationState::transitionMap)
;
}


// RTTR_REGISTRATION
// {
//     rttr::registration::class_<AnimationInfo>("AnimatorComponent")
//         .constructor<>()
//     (
//         rttr::policy::ctor::as_raw_ptr
//     )
//         .property("Name", &AnimationInfo::name)
//         .property("File Name", &AnimationInfo::fileName)
//         .property("Duration", &AnimationInfo::duration)
//         .property("Has Exit Time", &AnimationInfo::bHasExitTime)
//     ;
// }