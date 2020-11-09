
#pragma once

#include "rendering/VertexArray.h"
#include "Core/Graphics/Material/Material.h"

struct MeshInfo
{
	VertexArray* vertexArray;
	Material* material;

	~MeshInfo()
	{
		vertexArray->~VertexArray();
	}
};