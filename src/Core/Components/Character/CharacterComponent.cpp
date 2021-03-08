#include "CharacterComponent.h"

CharacterComponent::CharacterComponent(float movementSpeedIn)
	:movementSpeed(movementSpeedIn)
{

}

RTTR_REGISTRATION
{
	rttr::registration::class_<CharacterComponent>("CharacterComponent")
		.property("MovementSpeed", &CharacterComponent::movementSpeed)
	;
}