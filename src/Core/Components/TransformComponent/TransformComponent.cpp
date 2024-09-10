#include "TransformComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<TransformComponent>("TransformComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		.property("transform", &TransformComponent::transform)
		.property("Rotation Speed", &TransformComponent::rotSpeed)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 100.0f)
		)
	;
}

void TransformComponent::Initialize()
{
	m_entity->assign<TransformComponent>(this);
}
