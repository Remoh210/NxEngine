#pragma once

#include "Common/dataStructs/Map.h"
#include "Common/CommonTypes.h"
#include "Core/Graphics/Texture/Texture.h"

struct MaterialSpec
{
	NxMap<NString, NString> textureNames;
	NxMap<NString, Texture*> textures;
	NxMap<NString, float> floats;
	NxMap<NString, vec3> vectors;
	//NxMap<String, mat4> matrices;
};
