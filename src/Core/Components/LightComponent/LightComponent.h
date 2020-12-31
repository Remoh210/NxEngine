#pragma once
#include "Common/CommonTypes.h"

struct LightComponent
{
    LightComponent()
    {
        color = vec3(1.0f);
        intensity = 100.f;
        relativePosition = vec3(0.0f);
		direction = vec3(0.0f, -1.0f, 0.0f);
		//Default is point light
		lightType = 1;
    }

	// Point light constructor
    LightComponent(vec3 colorIn, float intensityIn, vec3 posIn)
        :color(colorIn)
        ,intensity(intensityIn)
        ,relativePosition(posIn)
    {
		lightType = 1;
		direction = vec3(0.0f);
    }

	// Directional light constructor
	LightComponent(vec3 colorIn, float intensityIn, vec3 posIn, vec3 directionIn)
		:color(colorIn)
		, intensity(intensityIn)
		, relativePosition(posIn)
		, direction(directionIn)
	{
		lightType = 0;
	}

	int lightType;
	float intensity;
    vec3 color;
    vec3 relativePosition;
 	vec3 direction; //For directional light
};
