#pragma once

#include "Common/dataStructs/Map.h"
#include "Common/CommonTypes.h"

struct Material
{
	Map<String, String> textureNames;
	Map<String, float> floats;
	Map<String, vec3> vectors;
	//Map<String, mat4> matrices;
};