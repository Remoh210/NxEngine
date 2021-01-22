#pragma once
#include <glm/glm.hpp>
#include "iCollisionBody.h"
namespace nPhysics
{
	class iVehicle : public iCollisonBody
	{
	public:
		virtual ~iVehicle() {}
		virtual void SetSteering(float steeringValue) = 0;
		virtual void SetBrakes(float brakesValue) = 0;
		virtual void MakeEngineGoNow(float howMuch) = 0;
		virtual void GetChassisTransform(glm::mat4& transformOut) = 0;
		virtual void GetWheelTransform(size_t index, glm::mat4& transformOut) = 0;
	};
}