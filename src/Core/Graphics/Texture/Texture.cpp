#include "Texture.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Texture>("Texture")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Texture ID", &Texture::GetId, &Texture::SetId)
		.property("Texture FileName", &Texture::GetFileName, &Texture::SetFileName)
	;
}