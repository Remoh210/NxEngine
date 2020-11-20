#pragma once

#include "Common/dataStructs/String.h"
#include "Core/ECS/Component.h"
#include "rendering/Texture.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Material/Material.h"
#include "Core/Graphics/Mesh/MeshInfo.h"


struct StaticMeshComponent : public Component<StaticMeshComponent>
{
	NString meshAssetFile; 
	Array<MeshInfo*> meshes;
	Shader* shader;
	int numInst = 0;
};