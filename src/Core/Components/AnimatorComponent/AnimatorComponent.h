#pragma once

#include "Core/Graphics/Animation/AnimationInfo.h"
#include "Core/Engine/Component/BaseComponent.h"
#include "Common/Common.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

struct AnimatorComponent : public BaseComponent
{
	NxMap<NString, AnimationInfo*> animations;

	AnimationInfo* currentAnimation;
	AnimationState InitialState;
	AnimationState currentState;

	NxMap<NString, AnimationState> animationStates;
};