#include <Common/Transform/Transform.h>
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

mat4 Transform::ToMatrix()
{
	mat4 trans = mat4(1.0f);
	trans = glm::translate(trans, position.ToVec());
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().x), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().y), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(rotation.ToVec().z), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, scale.ToVec());
	return trans;
}


RTTR_REGISTRATION
{
		rttr::registration::class_<Transform>("TransformComponent")
		.property("position", &Transform::position)
		.property("rotation", &Transform::rotation)
		.property("scale",    &Transform::scale)
	;
}