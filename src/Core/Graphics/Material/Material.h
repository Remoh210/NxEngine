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
		ambientOcclusion = 1.0f;
		metallic = 0.3f;
		roughness = 0.6f;

		//diffuseTexture = nullptr;
		//normalMap = nullptr;
		//metallicMap = nullptr;
		//roughnessMap = nullptr;
		//aoMap = nullptr;
	}


	Map<NString, Texture*> textures;

	////TODO: Make Map of textures maybe
	//Texture* diffuseTexture;
	//Texture* normalMap;
	//Texture* metallicMap;
	//Texture* roughnessMap;
	//Texture* aoMap;

	// color and alpha transparency
	vec4 color;

	//PBR properties
	float ambientOcclusion;
	float metallic;
	float roughness;
};