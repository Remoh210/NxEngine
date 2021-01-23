#pragma once

#include "Editor/EditorRenderContext.h"
#include "rendering/RenderDevice.h"
#include "Common/dataStructs/NxArray.h"
#include "Common/Transform/Transform.h"
#include "DebugShape.h"

class DebugRenderer
{
public:

	static void SetContext(EditorRenderContext* contextIn);

	static void SetShader(/*TODO*/);
    
	static void DrawDebugSphere(vec3 position, float time = 0, float radius = 1, vec3 color = vec3(1.f, 0.f, 0.f),
		uint32 sectorCount = 36, uint32 stackCount = 18);

    static void DrawDebugLine(vec3 start, vec3 end, float time, vec3 color);

	static void DrawQuad();

	static void Update(float dt);


private:
	static EditorRenderContext* editorContext;
	static NxArray<DebugShape*> ShapesToDraw;
	static Shader* shader;
	static DrawParams debugDrawParams;
};