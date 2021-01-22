#pragma once
#include "Common/Input/Input.h"


class AnimationState
{
public:
	AnimationState()
	{
	};

	struct AnimTransition
	{
		AnimTransition() {};
		AnimTransition(InputKey KeyIn)
			:conditionKey(KeyIn)
		{}

		virtual bool ShouldTansit(InputKey KeyIn) { return KeyIn == conditionKey; };
		InputKey conditionKey = InputKey::KEY_NONE;
	};

	struct StateDetails
	{
		StateDetails() :
			currentTime(0.0f),
			totalTime(0.0f),
			bHasExitTime(false),
			bExited(false),
			frameStepTime(0.0f) {};
		std::string name;
		float currentTime;		// Time (frame) in current animation
		float totalTime;		// Total time animation goes
		float frameStepTime;	// Number of seconds to 'move' the animation	
		// Returns true if time had to be reset
		// (for checking to see if the animation has finished or not)
		// TODO: Deal with running the animation backwards, perhaps?? 
		bool bExited;
		bool bHasExitTime;

		inline bool IncrementTime(bool bResetToZero = true)
		{
			bool bDidWeReset = false;
			bExited = false;

			this->currentTime += this->frameStepTime;
			//DEBUG_LOG_TEMP("%f", currentTime);
			if (this->currentTime >= this->totalTime)
			{
				this->currentTime = 0.0f;
				bDidWeReset = true;
				bExited = true;
			}

			return bDidWeReset;
		}

		bool operator == (const StateDetails& other) 
		{
			return other.name == this->name;
		}
	};


	// Extent Values for skinned mesh
	// These can be updated per frame, from the "update skinned mesh" call
	glm::vec3 minXYZ_from_SM_Bones;
	glm::vec3 maxXYZ_from_SM_Bones;
	// Store all the bones for this model, being updated per frame
	std::vector< glm::mat4x4 > vecObjectBoneTransformation;

	//std::vector<StateDetails> vecAnimationQueue;
	StateDetails activeAnimation;
	StateDetails nextAnimation;
	StateDetails PrevAnimation;

	NxMap<NString, AnimTransition> transitionMap;
	//StateDetails defaultAnimation;
};

struct AnimationInfo
{
	std::string name;
	std::string fileName;
	float duration;
	bool bHasExitTime;
	const aiScene* pAIScene;
};