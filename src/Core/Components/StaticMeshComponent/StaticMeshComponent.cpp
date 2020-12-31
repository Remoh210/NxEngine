#include "StaticMeshComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<StaticMeshComponent>("StaticMeshComponent")
		.property("Meshes", &StaticMeshComponent::meshes)
	;
}