#pragma once
#include "Common/Transform/Transform.h"
#include "Core/Graphics/Texture/Texture.h"
#include "Core/Graphics/Mesh/MeshInfo.h"

struct ImpostorComponent 
{
	ImpostorComponent()
	{

	}

	MeshInfo* mesh;
	VertexArray* vertexArray;
};