#include "StaticMeshComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<StaticMeshComponent>("StaticMeshComponent")
		.property("Is Visible", &StaticMeshComponent::bIsVisible)
		.property("Meshes", &StaticMeshComponent::meshes)
	;
}