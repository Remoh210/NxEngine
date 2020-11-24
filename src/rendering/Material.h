#pragma once

#include "Common/dataStructs/Map.h"
#include "Common/CommonTypes.h"
#include "rendering/Texture.h"

struct MaterialSpec
{
	Map<NString, NString> textureNames;
	Map<NString, Texture*> textures;
	Map<NString, float> floats;
	Map<NString, vec3> vectors;
	//Map<String, mat4> matrices;
};
