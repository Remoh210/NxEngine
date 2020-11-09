#pragma once

#include "Common/CommonTypes.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

#include "rendering/Texture.h"
#include "rendering/Shader.h"

struct Material
{
	Material()
	{
		color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

    Array<Texture*> diffuseTextures;
    Array<Texture*> normalMaps;

	// color and alpha transparency
	vec4 color;
};