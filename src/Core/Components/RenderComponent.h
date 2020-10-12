#pragma once

#include "Core/ECS/ECS.h"
#include "Common/Common.h"
#include "Common/CommonTypes.h"
#include <glad/gl.h>

struct RenderComponent : public Component<RenderComponent>
{
	string mesh;// MeshComp?
	string meshPath;
	string shader;// MeshComp?
	GLint shaderID;
	vec4 diffuse;

	bool bIsWireFrame;
	bool bIsVisible;
};