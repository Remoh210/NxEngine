#include "MeshInfo.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<MeshInfo>("MeshInfo")
		.constructor<>()
	(
		 rttr::policy::ctor::as_raw_ptr
	)
		.property("Material", &MeshInfo::material)
	;
}