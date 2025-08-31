#pragma once

//Remove?
#include "Common/dataStructs/Map.h"
#include "Core/Input/InputManager.h"
#include "rttr/registration.h"


struct aiScene;

struct AnimTransition
{
	AnimTransition() {}
	AnimTransition(int32 KeyIn)
		:conditionKey(KeyIn)
	{}

	virtual bool ShouldTransit(int32 KeyIn) { return KeyIn == conditionKey; }
	
	int32 conditionKey = static_cast<int32>(InputKey::KEY_NONE);
};

struct StateDetails
{
	StateDetails() :
		currentTime(0.0f),
		totalTime(0.0f),
		bHasExitTime(false),
		bExited(false),
		frameStepTime(0.0f) {}
	
	inline bool IncrementTime(bool bResetToZero = true)
	{
		bool bDidWeReset = false;
		bExited = false;

		this->currentTime += this->frameStepTime;
		if (this->currentTime >= this->totalTime)
		{
			this->currentTime = 0.0f;
			bDidWeReset = true;
			bExited = true;
		}

		return bDidWeReset;
	}

	bool operator == (const StateDetails& other) const 
	{
		return other.name == this->name;
	}

	std::string name;
	float currentTime;		// Time (frame) in current animation
	float totalTime;		// Total time animation goes
	float frameStepTime;	// Number of seconds to 'move' the animation	
	// Returns true if time had to be reset
	// (for checking to see if the animation has finished or not)
	bool bExited;
	bool bHasExitTime;

	RTTR_ENABLE()
};

class AnimationState
{
public:
	AnimationState()
	{
	}
	
	// Extent Values for skinned mesh
	// These can be updated per frame, from the "update skinned mesh" call
	glm::vec3 minXYZ_from_SM_Bones;
	glm::vec3 maxXYZ_from_SM_Bones;
	// Store all the bones for this model, being updated per frame
	std::vector< glm::mat4x4 > vecObjectBoneTransformation;

	
	StateDetails activeAnimation;
	StateDetails nextAnimation;
	StateDetails prevAnimation;

	NxMap<NString, AnimTransition> transitionMap;

	RTTR_ENABLE()
};

struct AnimationInfo
{
	std::string name;
	std::string fileName;
	float duration;
	bool bHasExitTime;
	const aiScene* pAIScene;

	RTTR_ENABLE()
};
