#pragma once

#include "Core/Graphics/Mesh/MeshInfo.h"

#include "Common/Common.h"
#include "Common/CommonTypes.h"
#include "Common/dataStructs/NxArray.h"

#include "assimp/scene.h"

struct BoneInfo
{
	mat4 BoneOffset;
	mat4 FinalTransformation;
	mat4 ObjectBoneTransformation;
};

struct SkeletalData
{
	unsigned int m_numberOfVertices = 0;
	unsigned int m_numberOfIndices = 0;
	unsigned int m_numberOfTriangles = 0;

	//NxArray<VertexBoneData> vecVertexBoneData;	//Bones;
	mat4 mGlobalInverseTransformation;

	NxMap<NString /*BoneName*/, unsigned int /*BoneIndex*/> m_mapBoneNameToBoneIndex;	//mMapping;
	NxArray<BoneInfo> mBoneInfo;
	unsigned int mNumBones = 0;	//mNums;
};

struct SkinnedMeshInfo
{
	SkeletalData skeletalData;
	MeshInfo* mesh;

	const aiScene* AiScene;	// mesh we update and draw
};