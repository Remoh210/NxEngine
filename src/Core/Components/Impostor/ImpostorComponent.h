#pragma once
#include "Common/Transform/Transform.h"
#include "Core/Graphics/Texture/Texture.h"
#include "Core/Engine/Component/BaseComponent.h"
#include "Core/Graphics/Mesh/MeshInfo.h"

struct ImpostorComponent : public BaseComponent
{
	MeshInfo* mesh;
	VertexArray* vertexArray;
};