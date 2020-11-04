#pragma once

#include "Core/ECS/Component.h"
#include "rendering/Texture.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include "Core/Graphics/Material/Material.h"

struct MeshInfo
{
	VertexArray* vertexArray;
	Material* material;
};

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
	Array<MeshInfo*> meshes;
	Shader* shader;
	int numInst = 0;
};