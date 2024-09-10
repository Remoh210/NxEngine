#pragma once

#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"

//DrawParams
#include "rendering/RenderDevice.h"

#include <rttr/registration>

struct SkinnedMeshComponent : public BaseComponent
{
	SkinnedMeshComponent(ECS::Entity* entity)
		:BaseComponent(entity)
	{	
	}

	SkinnedMeshComponent(ECS::Entity* entity, SkinnedMeshInfo* infoIn)
		:BaseComponent(entity)
		,skinnedMeshInfo(infoIn)
	{
	}

	SkinnedMeshInfo* skinnedMeshInfo;
	
	RTTR_ENABLE()
};