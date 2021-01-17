#include "Math.h"

RTTR_REGISTRATION
{
		rttr::registration::class_<vec3f>("vec3f")
		.property("data", &vec3f::Get, &vec3f::Set)
	;

		rttr::registration::class_<vec4f>("vec4f")
		.property("data", &vec4f::Get, &vec4f::Set)
	;
}

