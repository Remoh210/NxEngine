#include "CharacterComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<CharacterComponent>("CharacterComponent")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
		.property("MovementSpeed", &CharacterComponent::movementSpeed)
	;
}

void CharacterComponent::Initialize(ECS::Entity* entityIn)
{
	BaseComponent::Initialize(entityIn);

	entity->assign<CharacterComponent>(*this);
}
