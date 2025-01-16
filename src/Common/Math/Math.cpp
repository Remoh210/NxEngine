#include "Math.h"

RTTR_REGISTRATION
{
		rttr::registration::class_<vec3f>("vec3f")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("InternalData", &vec3f::GetInternal, &vec3f::SetInternal)
	;

	// 	 rttr::registration::class_<vec4f>("vec4f")
	// 	.property("data_vector", &vec4f::data)
	// ;
}

