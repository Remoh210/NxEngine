#pragma once
#include "CommonTypes.h"
#include <glm/glm.hpp>

struct Transform
{
	Transform()
	{
		position = vec3f(0.f);
		scale = vec3f(0.f);
		rotation = glm::quat(vec3f(0.f));
	}


	inline void AdjustRot(glm::vec3 adjAngleEuler, bool bIsDegrees = false)
	{
		if (bIsDegrees)
		{
			adjAngleEuler = glm::vec3(glm::radians(adjAngleEuler.x),
				glm::radians(adjAngleEuler.y),
				glm::radians(adjAngleEuler.z));
		}

		glm::quat rotationAdjust(adjAngleEuler);

		rotation *= rotationAdjust;

		return;
	}

	vec3f position;
	quat rotation;
	vec3f scale;
};