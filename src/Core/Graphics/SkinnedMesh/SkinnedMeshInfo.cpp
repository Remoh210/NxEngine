#include "SkinnedMeshInfo.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<SkinnedMeshInfo>("StaticMeshComponent")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Mesh", &SkinnedMeshInfo::mesh)
		;
}