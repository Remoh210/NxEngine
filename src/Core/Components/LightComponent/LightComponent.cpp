#include "LightComponent.h"



RTTR_REGISTRATION
{
	rttr::registration::class_<LightComponent>("LightComponent")
		.property("LightType", &LightComponent::lightType)
		.property("Intensity", &LightComponent::intensity)
				  (
					  rttr::metadata("Min", 0.0f),
					  rttr::metadata("Max", 6000.0f)
				  )
		.property("Direction", &LightComponent::direction)
		.property("Relative Position", &LightComponent::relativePosition)
	;

	rttr::registration::enumeration<LightType>("LightType")
	(
		rttr::value("Directional", LightType::Directional),
		rttr::value("Point", LightType::Point),
		rttr::value("Spot", LightType::Spot)
	);
}

void LightComponent::Initialize()
{
	//m_entity->assign<LightComponent>(this);
}
