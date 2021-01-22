#pragma once

#include<glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <string>

namespace nPhysics
{
	struct sRigidBodyDef
	{
		sRigidBodyDef() 
			: Mass(0.0f) 
			, Scale(1.0f, 1.0f, 1.0f)
			, Position(0.0f, 0.0f, 0.0f)
			, Velocity(0.0f, 0.0f, 0.0f)
			, Orientation(0.0f, 0.0f, 0.0f)
			, AngularVelocity(0.0f, 0.0f, 0.0f)
			, GameObjectName("undefined")
			, isPlayer(false)
			, isGhostShape(false)
		{

		}
		float Mass;
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec3 Scale;
		bool isPlayer;
		bool isGhostShape;
		std::string GameObjectName;
		glm::vec3 Orientation; //Euler Angles
		glm::vec3 AngularVelocity;
		glm::quat quatOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	};
}