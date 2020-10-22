#include "DebugRenderer.h"
#include "Core/Graphics/LineRenderer/LineRenderer.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/Application.h"

//HACK
#include <algorithm>

DebugRenderer::DebugRenderer(EditorRenderContext& contextIn)
    :editorContext(contextIn)
{
    //Create shader
        //Load and set shaders
    String LINE_SHADER_TEXT_FILE = Nx::FileSystem::GetPath("res/shaders/LineShader.glsl");
    String LineShaderText;
    Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
    shader = new Shader(*editorContext.GetRenderDevice(), LineShaderText);

    debugDrawParams.primitiveType = PRIMITIVE_TRIANGLES;
	debugDrawParams.shouldWriteDepth = true;
	debugDrawParams.depthFunc = DRAW_FUNC_LESS;
}

void DebugRenderer::DrawDebugSphere(vec3 position, float time = 0, float radius = 1, vec3 color = vec3(1.f, 0.f, 0.f))
{
    DebugShape SphereShape;
    SphereShape.vertexArray = new VertexArray(*editorContext.GetRenderDevice(), LineRenderer::CreateSphere(1, 36, 18), BufferUsage::USAGE_DYNAMIC_DRAW);
    //SphereShape.vertexArray->SetShader(shader)
    SphereShape.lifetime = time;
    SphereShape.transform.position = position;
    SphereShape.transform.scale = vec3(radius);
    ShapesToDraw.push_back(SphereShape);


    //if(!ShapeToRender.Find(SphereShape))



}

void DebugRenderer::Update(float dt)
{
    for(DebugShape& shape : ShapesToDraw)
    {
        
        //editorContext.RenderMesh(*shape.vertexArray, shape.transform.ToMatrix());

        editorContext.RenderPrimitives(shape.vertexArray, shader, shape.transform.ToMatrix(), debugDrawParams);
        
        shape.timeSinceCreated += dt;
        if(shape.timeSinceCreated > shape.lifetime)
        {
            //hack
            //ShapesToDraw.erase(std::remove(ShapesToDraw.begin(), ShapesToDraw.end(), shape), ShapesToDraw.end());
        }
    }
    //for(VAToRender)
    //Context->RenderMesh(DebugShape.VertexArray, ...)
}
