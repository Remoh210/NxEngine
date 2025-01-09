#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "Core/Engine/Component/BaseComponent.h"

#include <rttr/registration>

struct StaticMeshComponent : public BaseComponent 
{
	StaticMeshComponent()
	{
		Initialize();
	}
	
	StaticMeshComponent(ECS::Entity* entity)
	:BaseComponent(entity)
	{
		Initialize();
	}
	
	NString meshAssetFile; 
	NxArray<MeshInfo*> meshes;
	
	//TODO: Remove
	Shader* shader;
	int numInst = 1;
	bool bIsVisible = true;

	RTTR_ENABLE()
	
private:
	void Initialize();
};
