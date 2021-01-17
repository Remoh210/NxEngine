#pragma once

#include "Common/CommonTypes.h"
#include "Common/Math/Math.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

#include "Core/Graphics/Texture/Texture.h"
#include "rendering/Shader.h"

#include "rttr/registration"

struct Material
{
	Material()
	{
		color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		ambientOcclusion = 1.0f;
		metallic = 0.95f;
		roughness = 0.95f;
	}

	NxMap<NString, Texture*> textures;

	// color and alpha transparency
	vec4f color;

	//PBR properties
	float metallic;
	float roughness;
	float ambientOcclusion;

	RTTR_ENABLE();
};