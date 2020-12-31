#include "TransformComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<TransformComponent>("TransformComponent")
		.property("transform", &TransformComponent::transform)
		.property("Rotation Speed", &TransformComponent::rotSpeed)
				  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 100.0f)
				  )
	;
}