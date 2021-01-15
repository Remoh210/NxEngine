#pragma once

#include "Core/Graphics/Animation/AnimationInfo.h"
#include "Common/Common.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

struct AnimatorComponent
{
	NxMap<NString /*animation FRIENDLY name*/,
		AnimationInfo*> animationFriendlyNameTo_pScene;// Animations

	AnimationInfo* currentAnimation;
	AnimationState animationState;
};