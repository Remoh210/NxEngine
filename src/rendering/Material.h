#pragma once

#include "common/dataStructs/Map.h"
#include "common/CommonTypes.h"

struct Material
{
	Map<String, String> textureNames;
	Map<String, float> floats;
	Map<String, vec3> vectors;
	//Map<String, mat4> matrices;
};