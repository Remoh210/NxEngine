#include <Common/Transform/Transform.h>
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

mat4 Transform::ToMatrix()
{
	mat4 trans = mat4(1.0f);
	trans = glm::translate(trans, position);

	trans = glm::rotate(trans, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	trans = glm::scale(trans, scale);

	return trans;
}
