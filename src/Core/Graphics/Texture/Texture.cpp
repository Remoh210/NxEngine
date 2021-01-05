#include "Texture.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Texture>("Texture")
		.property("Texture ID", &Texture::GetId, &Texture::SetId)
	;
}