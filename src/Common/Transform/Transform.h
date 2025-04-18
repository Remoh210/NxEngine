#pragma once
#include <Common/CommonTypes.h>
#include <Common/Math/Math.h>
#include "rttr/registration"
#include "Common/dataStructs/NxArray.h"

struct Transform
{
	Transform()
	{
		position = vec3f();
		scale = vec3f(1.0f);
		rotation = vec3f();
	}

	Transform(vec3 posIn, vec3 rotIn = vec3(0.f), vec3 scaleIn = vec3(1.f))
		:position(posIn)
		,rotation(rotIn)
		,scale(scaleIn)
	{
	}

	mat4 ToMatrix();

	vec3 GetForwardVector();
	
	vec3f position;
	vec3f rotation;
	vec3f scale;

	RTTR_ENABLE()
};



