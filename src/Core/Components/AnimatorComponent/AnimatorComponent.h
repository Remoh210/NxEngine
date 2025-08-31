#pragma once

#include "Common/Common.h"
#include "Core/Graphics/Animation/AnimationInfo.h"
#include "Core/Engine/Component/BaseComponent.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

#include "rttr/registration"

struct AnimatorComponent : public BaseComponent
{
	
	AnimationInfo* currentAnimation;
	
	NString initialState;
	
	NxMap<NString, NString> animNameToFileMap;
	
	NxMap<NString, AnimationInfo*> animationMap;
	NxMap<NString, AnimationState> animationStates;

	void Initialize(ECS::Entity* entityIn) override;

	AnimationState currentState;

	RTTR_ENABLE(BaseComponent)
};