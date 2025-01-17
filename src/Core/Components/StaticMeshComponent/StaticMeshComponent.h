#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "Core/Engine/Component/BaseComponent.h"

#include <rttr/registration>

struct StaticMeshComponent : public BaseComponent 
{
	virtual void Initialize(ECS::Entity* entityIn) override;
	
	NString meshAssetFile; 
	NxArray<MeshInfo*> meshes;
	
	Shader* shader = nullptr;
	int numInst = 1;
	bool bIsVisible = true;
	bool bIsImported = false;

	RTTR_ENABLE(BaseComponent)
};
