#pragma once

#include "SkinnedMeshComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<SkinnedMeshComponent>("SkinnedMeshComponent")
		.property("Skinned Mesh", &SkinnedMeshComponent::skinnedMeshInfo)
	;
}
