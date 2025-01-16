#include "Material.h"

RTTR_REGISTRATION
{
		rttr::registration::class_<Material>("Material")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		.property("Textures", &Material::textures)
		.property("Metallic", &Material::metallic)
				  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 1.0f)
				  )
		.property("Roughness", &Material::roughness)
				  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 1.0f)
				  )
		.property("Ambient Occlusion", &Material::ambientOcclusion)
					  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 1.0f)
				      )
		.property("Color & Alpla", &Material::color)
					  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 1.0f)
					  )
	;
}