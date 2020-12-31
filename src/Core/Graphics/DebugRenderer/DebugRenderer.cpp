#include "DebugRenderer.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/Application.h"

//HACK
#include <algorithm>

DebugRenderer::DebugRenderer(EditorRenderContext& contextIn)
    :editorContext(contextIn)
{
    //Load and set shaders
	NString LINE_SHADER_TEXT_FILE ="res/shaders/DebugShapeShader.glsl";
	NString LineShaderText;
	Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
	shader = new Shader(editorContext.GetRenderDevice(), LineShaderText);

    debugDrawParams.primitiveType = PRIMITIVE_LINES;
	debugDrawParams.shouldWriteDepth = true;
	debugDrawParams.depthFunc = DRAW_FUNC_LESS;
}

void DebugRenderer::DrawDebugSphere(vec3 position, float time, float radius, vec3 color,
		uint32 sectorCount, uint32 stackCount)
{
    DebugShape* SphereShape = new DebugShape();

	IndexedModel model = PrimitiveGenerator::CreateSphere(1, sectorCount, stackCount, color);

	VertexArray* VA = new VertexArray(editorContext.GetRenderDevice(), model, BufferUsage::USAGE_DYNAMIC_DRAW);
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

	IndexedModel model = PrimitiveGenerator::CreateLine(color);

	VertexArray* VA = new VertexArray(editorContext.GetRenderDevice(), model, BufferUsage::USAGE_DYNAMIC_DRAW);
	VA->SetShader(shader);

	lineShape->vertexArray = VA;
	lineShape->drawParams = debugDrawParams;
    lineShape->lifetime = time;
    lineShape->transform.position = start;
    lineShape->transform.scale = end;
    ShapesToDraw.push_back(lineShape);
}

void DebugRenderer::DrawQuad()
{
    DebugShape* PlaneShape = new DebugShape();

	IndexedModel model = PrimitiveGenerator::CreateQuad();

	VertexArray* VA = new VertexArray(editorContext.GetRenderDevice(), model, BufferUsage::USAGE_DYNAMIC_DRAW);
	VA->SetShader(shader);

	PlaneShape->vertexArray = VA;
	PlaneShape->drawParams = debugDrawParams;
	PlaneShape->drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	PlaneShape->drawParams.faceCulling = FACE_CULL_FRONT_AND_BACK;
    PlaneShape->lifetime = 1000;
    //laneShape->transform.position = start;
    PlaneShape->transform.scale = vec3(10.0f);
    ShapesToDraw.push_back(PlaneShape);
}

void DebugRenderer::Update(float dt)
{
	NxArray<DebugShape*> DrawArray = ShapesToDraw;


    for(DebugShape* shape : DrawArray)
    {
        shape->timeSinceCreated += dt;
        if((shape->lifetime != 0.0f && shape->timeSinceCreated > shape->lifetime) || shape->bShouldDelete)
        {
			ArrayFuncs::Remove(ShapesToDraw, shape);
			//ShapesToDraw.Remove(shape);
            shape->~DebugShape();
        }
		else
		{
			editorContext.RenderDebugShapes(shape, shape->transform.ToMatrix());

			if (shape->lifetime == 0.0f)
			{
				shape->bShouldDelete = true;
			}
		}
    }
	
}
