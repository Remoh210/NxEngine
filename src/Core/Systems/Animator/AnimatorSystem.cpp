#include "AnimatorSystem.h"
#include "Common/CommonTypes.h"
#include "Common/Input/Input.h"
#include "Core/Components/Input/InputComponent.h"


namespace ECS 
{
	void AnimatorSystem::tick(class World *world, float deltaTime)
	{
		world->each
			<
			TransformComponent, 
			SkinnedMeshComponent, 
			AnimatorComponent,
		    InputComponent
			>
			([&](
			Entity *ent, 
			ComponentHandle<TransformComponent> transform,
			ComponentHandle<SkinnedMeshComponent> skinnedMesh, 
			ComponentHandle<AnimatorComponent> animatorComp,
			ComponentHandle<InputComponent> InputComp) -> void
		{
			AnimationInfo* curAnim = animatorComp->animations[animatorComp->currentState.activeAnimation.name];
			animatorComp->currentState.activeAnimation.totalTime = GetDurationInSec(curAnim);

			//if (animatorComp->currentState.activeAnimation.name != animatorComp->currentState.PrevAnimation.name)
			//{
			//	animatorComp->currentState.activeAnimation.currentTime = 0.0f;
			//}

			auto oldState = animatorComp->currentState;

			animatorComp->currentState.activeAnimation.totalTime = GetDurationInSec(curAnim);
			animatorComp->currentState.activeAnimation.frameStepTime = deltaTime;
			animatorComp->currentState.activeAnimation.IncrementTime();

			std::vector< glm::mat4x4 > vecOffsets;
			BoneTransform(skinnedMesh->skinnedMeshInfo, curAnim, animatorComp->currentState, skinnedMesh->skinnedMeshInfo->vecFinalTransformation, skinnedMesh->skinnedMeshInfo->vecObjectBoneTransformation, vecOffsets);

			if (animatorComp->currentState.transitionMap.size() > 0)
			{
				for (auto it : animatorComp->currentState.transitionMap)
				{
					if (it.second.ShouldTansit(InputComp->GetKeyPressed()))
					{
						animatorComp->currentState = animatorComp->animationStates[it.first];
						return;
					}
				}
			}
			
			bool bHasExitTime = animatorComp->currentState.activeAnimation.bHasExitTime;
			bool bHasExited = animatorComp->currentState.activeAnimation.bExited;
			if (InputComp->GetKeyPressed() == InputKey::KEY_NONE)
			{
				if (bHasExitTime && !bHasExited) { return; }

				if (animatorComp->currentState.activeAnimation.bHasExitTime)
				{
					//Do blending?
				}

				NString NextState = animatorComp->currentState.nextAnimation.name;
				if (NextState != "")
				{
					animatorComp->currentState = animatorComp->animationStates[NextState];
				}
				else
				{
					animatorComp->currentState = animatorComp->InitialState;
				}
			}

			animatorComp->currentState.prevAnimation = oldState.activeAnimation;	
		});
	}


	void AnimatorSystem::BoneTransform(
		SkinnedMeshInfo* skinnedMesh,
		AnimationInfo* animationInfo,
		AnimationState& animState,
		std::vector<glm::mat4> &FinalTransformation,
		std::vector<glm::mat4> &Globals,
		std::vector<glm::mat4> &Offsets)
	{
		glm::mat4 Identity(1.0f);

		float TicksPerSecond = static_cast<float>(skinnedMesh->AiScene->mAnimations[0]->mTicksPerSecond != 0 ?
			skinnedMesh->AiScene->mAnimations[0]->mTicksPerSecond : 25.0);

		float TimeInSeconds = animState.activeAnimation.currentTime;
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, animationInfo->pAIScene->mAnimations[0]->mDuration);

		// use the "animation" file to look up these nodes
		// (need the matOffset information from the animation file)
		ReadNodeHeirarchy(AnimationTime, skinnedMesh, skinnedMesh->AiScene->mRootNode, animationInfo, Identity);

		int numBones = skinnedMesh->skeletalData.mNumBones;
		FinalTransformation.resize(numBones);
		Globals.resize(numBones);
		Offsets.resize(numBones);

		for (unsigned int BoneIndex = 0; BoneIndex < numBones; BoneIndex++)
		{
			FinalTransformation[BoneIndex] = skinnedMesh->skeletalData.mBoneInfo[BoneIndex].FinalTransformation;
			Globals[BoneIndex] = skinnedMesh->skeletalData.mBoneInfo[BoneIndex].ObjectBoneTransformation;
			Offsets[BoneIndex] = skinnedMesh->skeletalData.mBoneInfo[BoneIndex].BoneOffset;
		}
	}

	void AnimatorSystem::ReadNodeHeirarchy(
		float AnimationTime,
		SkinnedMeshInfo* skinnedMesh,
		const aiNode * pNode,
		AnimationInfo* animationInfo,
		const glm::mat4 & ParentTransformMatrix)
	{

		aiString NodeName(pNode->mName.data);

		// Original version picked the "main scene" animation...
		//const aiAnimation* pAnimation = skinnedMesh->AiScene->mAnimations[0];

		//TODO: remoce Cast
		const aiAnimation* pAnimation = reinterpret_cast<const aiAnimation*>(animationInfo->pAIScene->mAnimations[0]);


		//aiMatrix4x4 NodeTransformation;

		// Transformation of the node in bind pose
		glm::mat4 NodeTransformation = AIMatrixToGLMMatrix(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = this->FindNodeAnimationChannel(pAnimation, NodeName);

		if (pNodeAnim)
		{
			// Get interpolated scaling
			glm::vec3 scale;
			CalcGLMInterpolatedScaling(AnimationTime, pNodeAnim, scale);
			glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

			// Get interpolated rotation (quaternion)
			glm::quat ori;
			CalcGLMInterpolatedRotation(AnimationTime, pNodeAnim, ori);
			glm::mat4 RotationM = glm::mat4_cast(ori);

			// Get interpolated position 
			glm::vec3 pos;
			CalcGLMInterpolatedPosition(AnimationTime, pNodeAnim, pos);
			glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), pos);


			// Combine the above transformations
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}

		glm::mat4 ObjectBoneTransformation = ParentTransformMatrix * NodeTransformation;

		std::map<std::string, unsigned int>::iterator it = skinnedMesh->skeletalData.m_mapBoneNameToBoneIndex.find(std::string(NodeName.data));
		if (it != skinnedMesh->skeletalData.m_mapBoneNameToBoneIndex.end())
		{
			unsigned int BoneIndex = it->second;
			skinnedMesh->skeletalData.mBoneInfo[BoneIndex].ObjectBoneTransformation = ObjectBoneTransformation;
			skinnedMesh->skeletalData.mBoneInfo[BoneIndex].FinalTransformation = skinnedMesh->skeletalData.mGlobalInverseTransformation
				* ObjectBoneTransformation
				* skinnedMesh->skeletalData.mBoneInfo[BoneIndex].BoneOffset;

		}
		else
		{
			int breakpoint = 0;
		}

		for (unsigned int ChildIndex = 0; ChildIndex != pNode->mNumChildren; ChildIndex++)
		{
			this->ReadNodeHeirarchy(AnimationTime, skinnedMesh,
				pNode->mChildren[ChildIndex], animationInfo, ObjectBoneTransformation);
		}
	}


	float AnimatorSystem::GetDurationInSec(const AnimationInfo* animInfo)
	{

		// This is scaling the animation from 0 to 1
		return animInfo->pAIScene->mAnimations[0]->mDuration / (float)animInfo->pAIScene->mAnimations[0]->mTicksPerSecond;
	}

	const aiNodeAnim* AnimatorSystem::FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString boneName)
	{
		for (unsigned int ChannelIndex = 0; ChannelIndex != pAnimation->mNumChannels; ChannelIndex++)
		{
			if (pAnimation->mChannels[ChannelIndex]->mNodeName == boneName)
			{
				return pAnimation->mChannels[ChannelIndex];
			}
		}
		return 0;
	}

	unsigned int AnimatorSystem::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int RotationKeyIndex = 0; RotationKeyIndex != pNodeAnim->mNumRotationKeys - 1; RotationKeyIndex++)
		{
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[RotationKeyIndex + 1].mTime)
			{
				return RotationKeyIndex;
			}
		}

		return 0;
	}

	unsigned int AnimatorSystem::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int PositionKeyIndex = 0; PositionKeyIndex != pNodeAnim->mNumPositionKeys - 1; PositionKeyIndex++)
		{
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[PositionKeyIndex + 1].mTime)
			{
				return PositionKeyIndex;
			}
		}

		return 0;
	}

	unsigned int AnimatorSystem::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int ScalingKeyIndex = 0; ScalingKeyIndex != pNodeAnim->mNumScalingKeys - 1; ScalingKeyIndex++)
		{
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[ScalingKeyIndex + 1].mTime)
			{
				return ScalingKeyIndex;
			}
		}

		return 0;
	}

	void AnimatorSystem::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion &out)
	{
		if (pNodeAnim->mNumRotationKeys == 1)
		{
			out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		unsigned int NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
		out = out.Normalize();

		return;
	}

	void AnimatorSystem::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out)
	{
		if (pNodeAnim->mNumPositionKeys == 1)
		{
			out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		unsigned int NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		out = (EndPosition - StartPosition) * Factor + StartPosition;

		return;
	}

	void AnimatorSystem::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out)
	{
		if (pNodeAnim->mNumScalingKeys == 1)
		{
			out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		unsigned int NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		out = (EndScale - StartScale) * Factor + StartScale;

		return;
	}

	void AnimatorSystem::CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat &out)
	{
		if (pNodeAnim->mNumRotationKeys == 1)
		{
			out.w = pNodeAnim->mRotationKeys[0].mValue.w;
			out.x = pNodeAnim->mRotationKeys[0].mValue.x;
			out.y = pNodeAnim->mRotationKeys[0].mValue.y;
			out.z = pNodeAnim->mRotationKeys[0].mValue.z;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		unsigned int NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		if (Factor < 0.0f) Factor = 0.0f;
		if (Factor > 1.0f) Factor = 1.0f;
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

		glm::quat StartGLM = glm::quat(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
		glm::quat EndGLM = glm::quat(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

		out = glm::slerp(StartGLM, EndGLM, Factor);

		out = glm::normalize(out);

		return;
	}

	void AnimatorSystem::CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
	{
		if (pNodeAnim->mNumPositionKeys == 1)
		{
			out.x = pNodeAnim->mPositionKeys[0].mValue.x;
			out.y = pNodeAnim->mPositionKeys[0].mValue.y;
			out.z = pNodeAnim->mPositionKeys[0].mValue.z;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		unsigned int NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		if (Factor < 0.0f) Factor = 0.0f;
		if (Factor > 1.0f) Factor = 1.0f;
		const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		glm::vec3 start = glm::vec3(StartPosition.x, StartPosition.y, StartPosition.z);
		glm::vec3 end = glm::vec3(EndPosition.x, EndPosition.y, EndPosition.z);

		out = (end - start) * Factor + start;

		return;
	}

	void AnimatorSystem::CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
	{
		if (pNodeAnim->mNumScalingKeys == 1)
		{
			out.x = pNodeAnim->mScalingKeys[0].mValue.x;
			out.y = pNodeAnim->mScalingKeys[0].mValue.y;
			out.z = pNodeAnim->mScalingKeys[0].mValue.z;
			return;
		}

		unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		unsigned int NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		if (Factor < 0.0f) Factor = 0.0f;
		if (Factor > 1.0f) Factor = 1.0f;
		const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		glm::vec3 start = glm::vec3(StartScale.x, StartScale.y, StartScale.z);
		glm::vec3 end = glm::vec3(EndScale.x, EndScale.y, EndScale.z);
		out = (end - start) * Factor + start;

		return;
	}

	glm::mat4 AnimatorSystem::AIMatrixToGLMMatrix(const aiMatrix4x4 &mat)
	{
		return glm::mat4(mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c4, mat.d4);
	}
}// namespace ECS