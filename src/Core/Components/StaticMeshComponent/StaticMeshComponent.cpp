#include "StaticMeshComponent.h"
#include "Core/Application/AssetManager/AssetManager.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<StaticMeshComponent>("StaticMeshComponent")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
		.property("Is Visible", &StaticMeshComponent::bIsVisible)
		.property("Num Instances", &StaticMeshComponent::numInst)
		.property("Is Imported", &StaticMeshComponent::bIsImported)
		.property("Mesh Asset File", &StaticMeshComponent::meshAssetFile)
		.property("Meshes", &StaticMeshComponent::meshes)
	;
}

void StaticMeshComponent::Initialize(ECS::Entity* entityIn)
{
	BaseComponent::Initialize(entityIn);
	
	if(!entity)
	{
		return;
	}

	// no support for raw models for now
	if(bIsImported && !meshAssetFile.empty())
	{
		meshes = AssetManager::ImportModel(meshAssetFile);
		shader = ShaderManager::GetMainShader();
		entity->assign<StaticMeshComponent>(*this);
	}
}
