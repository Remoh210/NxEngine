#pragma once
#include <Common/CommonTypes.h>

struct Transform
{
	Transform()
	{
		position = vec3(0.0f);
		scale = vec3(1.0f);
		rotation = vec3(0.0f, 0.0f, 0.0f);
	}

	Transform(vec3 posIn, vec3 rotIn = vec3(0.f), vec3 scaleIn = vec3(1.f))
		:position(posIn)
		,rotation(rotIn)
		,scale(scaleIn)
	{
	}


	//inline void AdjustRot(glm::vec3 adjAngleEuler, bool bIsDegrees = false)
	//{
	//	if (bIsDegrees)
	//	{
	//		adjAngleEuler =vec3(glm::radians(adjAngleEuler.x),
	//			glm::radians(adjAngleEuler.y),
	//			glm::radians(adjAngleEuler.z));
	//	}

	//	glm::quat rotationAdjust(adjAngleEuler);

	//	rotation *= rotationAdjust;

	//	return;
	//}

	mat4 ToMatrix();

	vec3 position;
	vec3 rotation;
	vec3 scale;
};
