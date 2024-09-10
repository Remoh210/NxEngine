#pragma once
#include "Core/Engine/Component/BaseComponent.h"
#include "Common/CommonTypes.h"
#include "Common/Math/Math.h"

#include <rttr/registration.h>

enum class LightType
{
	Directional = 0,
	Point = 1,
	Spot = 2
};


struct LightComponent : public BaseComponent
{
    LightComponent(ECS::Entity* entity)
	    :BaseComponent(entity)
    {
        color = vec3(1.0f);
        intensity = 100.f;
        relativePosition = vec3(0.0f);
		direction = vec3(0.0f, -1.0f, 0.0f);
		//Default is point light
		lightType = LightType::Point;
    	Initialize();
    }

	// Point light constructor
    LightComponent(ECS::Entity* entity, vec3 colorIn, float intensityIn, vec3 posIn)
	     :BaseComponent(entity),
	     color(colorIn)
        ,intensity(intensityIn)
        ,relativePosition(posIn)
    {
		lightType = LightType::Point;
		direction = vec3(0.0f);
    	Initialize();
    }

	// Directional light constructor
	LightComponent(ECS::Entity* entity, vec3 colorIn, float intensityIn, vec3 posIn, vec3 directionIn)
		 :BaseComponent(entity),
		 color(colorIn)
		, intensity(intensityIn)
		, relativePosition(posIn)
		, direction(directionIn)
	{
		lightType = LightType::Directional;
    	Initialize();
	}

	LightType lightType;
	float intensity;
    vec3f color;
    vec3f relativePosition;
 	vec3f direction; //For directional light

	RTTR_ENABLE()

private:
	void Initialize();
};
