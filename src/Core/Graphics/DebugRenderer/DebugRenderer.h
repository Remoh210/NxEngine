#pragma once

#include "Editor/EditorRenderContext.h"
#include "rendering/RenderDevice.h"
#include "Common/dataStructs/Array.h"
#include "Common/Transform/Transform.h"
#include "DebugShape.h"

class DebugRenderer
{
public:

    DebugRenderer(EditorRenderContext& contextIn);
    
    void DrawDebugSphere(vec3 position, float time, float radius, vec3 color);

    void DrawDebugLine(vec3 start, vec3 end, float time, vec3 color);

    void Update(float dt);


private:
    EditorRenderContext& editorContext;
    Array<DebugShape*> ShapesToDraw;
    Shader* shader;
    DrawParams debugDrawParams;
};