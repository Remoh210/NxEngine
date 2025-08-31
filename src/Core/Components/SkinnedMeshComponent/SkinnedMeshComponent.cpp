#pragma once

#include "SkinnedMeshComponent.h"

#include "Core/Application/AssetManager/AssetManager.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<SkinnedMeshComponent>("SkinnedMeshComponent")
		.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
		.property("Is Visible", &SkinnedMeshComponent::bIsVisible)
		.property("Mesh Asset File", &SkinnedMeshComponent::meshAssetFile)
	;
}


void SkinnedMeshComponent::Initialize(ECS::Entity* entityIn)
{
	BaseComponent::Initialize(entityIn);
	
	if(!entity)
	{
		return;
	}

	// no support for raw models for now
	if(!meshAssetFile.empty())
	{
		skinnedMeshInfo = AssetManager::ImportModelSkeletal(meshAssetFile);
		entity->assign<SkinnedMeshComponent>(*this);
	}
}