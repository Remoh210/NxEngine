#pragma once

#include "Common/dataStructs/Map.h"
#include "Common/CommonTypes.h"

struct Material
{
	Map<NString, NString> textureNames;
	Map<NString, float> floats;
	Map<NString, vec3> vectors;
	//Map<String, mat4> matrices;
};
