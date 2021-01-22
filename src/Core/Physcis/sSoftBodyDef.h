#pragma once
#include <glm.hpp>
#include <vector>

namespace nPhysics
{
	struct sSoftBodyNodeDef
	{
		sSoftBodyNodeDef(const glm::vec3& position, float mass)
			: Position(position)
			, Mass(mass)
		{

		}

		glm::vec3 Position;
		float Mass;
	};
	struct sSoftBodyDef
	{
		float SpringConstant;
		std::vector<sSoftBodyNodeDef> Nodes;
		std::vector<std::pair<size_t, size_t>> Springs;
	};
}