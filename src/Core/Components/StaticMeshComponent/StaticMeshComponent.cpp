#include "StaticMeshComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<StaticMeshComponent>("StaticMeshComponent")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
		.property("Is Visible", &StaticMeshComponent::bIsVisible)
		.property("Meshes", &StaticMeshComponent::meshes)
	;
}

void StaticMeshComponent::Initialize()
{
}
