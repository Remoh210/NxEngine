#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Texture.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Material/Material.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "rttr/registration"

struct StaticMeshComponent
{
	NString meshAssetFile; 
	Array<MeshInfo*> meshes;
	Shader* shader;
	int numInst = 1;

	RTTR_ENABLE();
};


RTTR_REGISTRATION
{
	rttr::registration::class_<StaticMeshComponent>("StaticMeshComponent")
		.property("meshAssetFile", &StaticMeshComponent::meshAssetFile)
		.property("numInst", &StaticMeshComponent::numInst)
	;




}