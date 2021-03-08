#include <Common/Transform/Transform.h>
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

mat4 Transform::ToMatrix()
{
	mat4 trans = mat4(1.0f);
	trans = glm::translate(trans, position.ToVec());
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().z), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().y), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().x), glm::vec3(1.0f, 0.0f, 0.0f));
	
	
	trans = glm::scale(trans, scale.ToVec());
	return trans;
}


vec3 Transform::GetForwardVector()
{

	vec4 forwModel = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	vec3 rotInRadians(glm::radians(rotation.ToVec().x), glm::radians(rotation.ToVec().y), glm::radians(rotation.ToVec().z));
	quat qOrientation(rotInRadians);

	mat4 matOrientation = glm::mat4(qOrientation);

	vec4 vecForwardDirection_WorldSpace = matOrientation * forwModel;

	// optional normalize
	vecForwardDirection_WorldSpace = glm::normalize(vecForwardDirection_WorldSpace);
	glm::vec3 veccForw(vecForwardDirection_WorldSpace.x, vecForwardDirection_WorldSpace.y, vecForwardDirection_WorldSpace.z);

	DEBUG_LOG_TEMP("veccForw: X: %f, Y: %f, Z: %f", veccForw.x, veccForw.y, veccForw.z);
	return veccForw;
}

RTTR_REGISTRATION
{
		rttr::registration::class_<Transform>("TransformComponent")
		.property("position", &Transform::position)
		.property("rotation", &Transform::rotation)
		.property("scale",    &Transform::scale)
	;
}