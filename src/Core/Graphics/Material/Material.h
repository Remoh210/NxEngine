#pragma once

#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"

#include "rendering/Texture.h"
#include "rendering/Shader.h"

struct Material
{
    NString Name = "NULL_MATERIAL";
    Array<Texture*> diffuseTextures;
    Array<Texture*> normalTextures;
    Shader* shader;
};