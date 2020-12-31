#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "rttr/registration"

struct StaticMeshComponent
{
	StaticMeshComponent()
	{
		bool bIsVisible = true;
	}
	NString meshAssetFile; 
	NxArray<MeshInfo*> meshes;
	//TODO: Remove
	Shader* shader;
	int numInst = 1;
	bool bIsVisible;

	RTTR_ENABLE();
};