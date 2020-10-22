#pragma once

#include "Editor/EditorRenderContext.h"
#include "rendering/RenderDevice.h"
#include "Common/dataStructs/Array.h"
#include "Common/Transform/Transform.h"
#include "rendering/VertexArray.h"

struct DebugShape
{
   float lifetime = 0;
   float timeSinceCreated = 0.0f;
   VertexArray* vertexArray;
   Transform transform;
   ~DebugShape()
   {
       delete vertexArray;
       DEBUG_LOG_TEMP("Debug Shape Destructor Was Called");
   } 
};

class DebugRenderer
{
public:


    DebugRenderer(EditorRenderContext& contextIn);
    
    void DrawDebugSphere(vec3 position, float time, float radius, vec3 color);

    void Update(float dt);


private:
    EditorRenderContext& editorContext;
    Array<DebugShape> ShapesToDraw;
    Shader* shader;
    DrawParams debugDrawParams;
};