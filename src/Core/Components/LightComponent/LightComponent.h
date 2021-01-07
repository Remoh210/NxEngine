#pragma once
#include "Common/CommonTypes.h"
#include "Common/Math/Math.h"

#include "rttr/registration.h"

enum class LightType
{
	Directional = 0,
	Point = 1,
	Spot = 2
};


struct LightComponent
{
    LightComponent()
    {
        color = vec3(1.0f);
        intensity = 100.f;
        relativePosition = vec3(0.0f);
		direction = vec3(0.0f, -1.0f, 0.0f);
		//Default is point light
		lightType = LightType::Point;
    }

	// Point light constructor
    LightComponent(vec3 colorIn, float intensityIn, vec3 posIn)
        :color(colorIn)
        ,intensity(intensityIn)
        ,relativePosition(posIn)
    {
		lightType = LightType::Point;
		direction = vec3(0.0f);
    }

	// Directional light constructor
	LightComponent(vec3 colorIn, float intensityIn, vec3 posIn, vec3 directionIn)
		:color(colorIn)
		, intensity(intensityIn)
		, relativePosition(posIn)
		, direction(directionIn)
	{
		lightType = LightType::Directional;
	}

	LightType lightType;
	float intensity;
    vec3f color;
    vec3f relativePosition;
 	vec3f direction; //For directional light

	RTTR_ENABLE();
};
