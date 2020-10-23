#include "DebugRenderer.h"
#include "Core/Graphics/LineRenderer/LineRenderer.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/Application.h"

//HACK
#include <algorithm>

DebugRenderer::DebugRenderer(EditorRenderContext& contextIn)
    :editorContext(contextIn)
{
    //Load and set shaders
	String LINE_SHADER_TEXT_FILE = Nx::FileSystem::GetPath("res/shaders/DebugShapeShader.glsl");
	String LineShaderText;
	Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
	shader = new Shader(*editorContext.GetRenderDevice(), LineShaderText);

    debugDrawParams.primitiveType = PRIMITIVE_LINES;
	debugDrawParams.shouldWriteDepth = true;
	debugDrawParams.depthFunc = DRAW_FUNC_LESS;
}

void DebugRenderer::DrawDebugSphere(vec3 position, float time = 0, float radius = 1, vec3 color = vec3(1.f, 0.f, 0.f))
{
    DebugShape* SphereShape = new DebugShape();

	IndexedModel model = LineRenderer::CreateSphere(1, 36, 18, color);

	VertexArray* VA = new VertexArray(*editorContext.GetRenderDevice(), model, BufferUsage::USAGE_DYNAMIC_DRAW);
	VA->SetShader(shader);

	SphereShape->vertexArray = VA;
	SphereShape->drawParams = debugDrawParams;
    SphereShape->lifetime = time;
    SphereShape->transform.position = position;
    SphereShape->transform.scale = vec3(radius);
    ShapesToDraw.push_back(SphereShape);

}

void DebugRenderer::DrawDebugLine(vec3 start, vec3 end, float time = 0, vec3 color = vec3(1.f, 0.f, 0.f))
{
    DebugShape* lineShape = new DebugShape();

	IndexedModel model = LineRenderer::CreateLine(color);

	VertexArray* VA = new VertexArray(*editorContext.GetRenderDevice(), model, BufferUsage::USAGE_DYNAMIC_DRAW);
	VA->SetShader(shader);

	lineShape->vertexArray = VA;
	lineShape->drawParams = debugDrawParams;
    lineShape->lifetime = time;
    lineShape->transform.position = start;
    lineShape->transform.scale = end;
    ShapesToDraw.push_back(lineShape);
}

void DebugRenderer::Update(float dt)
{
    for(DebugShape* shape : ShapesToDraw)
    {
		editorContext.RenderDebugShapes(shape, shape->transform.ToMatrix());
        shape->timeSinceCreated += dt;
        if(shape->timeSinceCreated > shape->lifetime)
        {
            //hack
            //ShapesToDraw.erase(std::remove(ShapesToDraw.begin(), ShapesToDraw.end(), shape), ShapesToDraw.end());
        }
    }
}
