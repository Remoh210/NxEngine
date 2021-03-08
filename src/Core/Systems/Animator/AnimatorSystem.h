#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Components/SkinnedMeshComponent/SkinnedMeshComponent.h"
#include "Core/Components/AnimatorComponent/AnimatorComponent.h"

namespace ECS
{
	class AnimatorSystem : public EntitySystem
	{
	public:
		AnimatorSystem() {}

		virtual void tick(class World *world, float deltaTime);

		void BoneTransform(
			SkinnedMeshInfo* skinnedMesh,
			AnimationInfo* animationInfo,
			AnimationState& animState,
			std::vector<glm::mat4> &FinalTransformation,
			std::vector<glm::mat4> &Globals,
			std::vector<glm::mat4> &Offsets);

		void ReadNodeHeirarchy(
			float AnimationTime,
			SkinnedMeshInfo* skinnedMesh,
			const aiNode * pNode,
			AnimationInfo* animationInfo,
			const glm::mat4 & ParentTransformMatrix);

		const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString boneName);
		float GetDurationInSec(const AnimationInfo* animInfo);
		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion &out);
		void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out);
		void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out);
		void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat &out);
		void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out);
		void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out);

	private:
		//TODO: Move to common place since it also is used in AssetLoader
		inline glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat);
	};
}
