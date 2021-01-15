#include "SkinnedMeshInfo.h"

RTTR_REGISTRATION
{
	//rttr::registration::class_<BoneInfo>("BoneInfo")
	//	.property("Is Visible", &BoneInfo::bIsVisible)
	//	.property("Meshes", &BoneInfo::meshes)
	//;

	rttr::registration::class_<SkeletalData>("SkeletalData")
		.property("Bones", &SkeletalData::m_mapBoneNameToBoneIndex);

	rttr::registration::class_<SkinnedMeshInfo>("StaticMeshComponent")
		.property("Mesh", &SkinnedMeshInfo::mesh)
		.property("Skeletal Data", &SkinnedMeshInfo::skeletalData)
		;
}