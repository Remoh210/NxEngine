#pragma once

#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"

//DrawParams
#include "rendering/RenderDevice.h"

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