#pragma once

#include "rendering/VertexArray.h"
#include "Core/Graphics/Material/Material.h"

#include "rttr/registration"

struct MeshInfo
{
	VertexArray* vertexArray = nullptr;
	Material* material = nullptr;

	~MeshInfo()
	{
		vertexArray->~VertexArray();
	}

	DrawParams drawParams;

	RTTR_ENABLE()
};