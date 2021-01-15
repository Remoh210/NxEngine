#pragma once

#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"

#include "rttr/registration"

struct SkinnedMeshComponent
{
	SkinnedMeshComponent()
	{	
	}

	SkinnedMeshComponent(SkinnedMeshInfo* infoIn)
		:skinnedMeshInfo(infoIn)
	{
	}

	SkinnedMeshInfo* skinnedMeshInfo;

	RTTR_ENABLE();
};