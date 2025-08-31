#pragma once

#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"

//DrawParams
#include "rendering/RenderDevice.h"

#include <rttr/registration>

struct SkinnedMeshComponent : public BaseComponent
{
	SkinnedMeshComponent()
	{	
	}
	
	SkinnedMeshComponent(ECS::Entity* entity)
		:BaseComponent(entity)
	{	
	}

	SkinnedMeshComponent(ECS::Entity* entity, SkinnedMeshInfo* infoIn)
		:BaseComponent(entity)
		,skinnedMeshInfo(infoIn)
	{
	}

	NString meshAssetFile;
	bool bIsVisible = true;
	SkinnedMeshInfo* skinnedMeshInfo = nullptr;

	void Initialize(ECS::Entity* entityIn);
	
	RTTR_ENABLE(BaseComponent)
};