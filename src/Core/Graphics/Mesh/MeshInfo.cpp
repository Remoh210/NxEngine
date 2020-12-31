#include "MeshInfo.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<MeshInfo>("MeshInfo")
		.property("Material", &MeshInfo::material)
	;
}