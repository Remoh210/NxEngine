#pragma once
#include "Common/CommonTypes.h"

struct LightComponent
{
    LightComponent()
    {
        color = vec3(1.0f);
        intensity = 1000.f;
        relativePosition = vec3(0.f);
    }

    LightComponent(vec3 colorIn, float intensityIn, vec3 posIn)
        :color(colorIn)
        ,intensity(intensityIn)
        ,relativePosition(posIn)
    {
    }

    vec3 color;
    float intensity;
    vec3 relativePosition;
};
