
#pragma once

#include "rendering/VertexArray.h"
#include "Core/Graphics/Material/Material.h"

struct MeshInfo
{
	VertexArray* vertexArray = nullptr;
	Material* material = nullptr;

	~MeshInfo()
	{
		vertexArray->~VertexArray();
	}
};