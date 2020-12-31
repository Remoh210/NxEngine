#include "Texture.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Texture>("Texture")
		.property("Texture", &Texture::GetId, &Texture::SetId)
	;
}