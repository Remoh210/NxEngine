#include "CharacterComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<CharacterComponent>("CharacterComponent")
		.property("MovementSpeed", &CharacterComponent::movementSpeed)
	;
}