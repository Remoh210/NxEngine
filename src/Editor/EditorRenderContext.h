#pragma once

#include "rendering/RenderDevice.h"
#include "rendering/RenderContext.h"
#include "rendering/Shader.h"
#include "rendering/Sampler.h"
#include "rendering/VertexArray.h"
#include "Common/CommonTypes.h"
#include "Common/Transform/Transform.h"
#include "Common/dataStructs/Map.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"
#include <Core/Camera/Camera.h>
#include <Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h>
#include "Core/Graphics/DebugRenderer/DebugShape.h"

class EditorRenderContext : public RenderContext
{
public:
	EditorRenderContext(RenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn,
	        Sampler& samplerIn, const mat4& perspectiveIn, Camera* CameraIn);
	inline void RenderMesh(VertexArray& vertexArray, Texture& texture, const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(&vertexArray, &texture)].push_back(perspective * mainCamera->GetViewMatrix() * transformIn);
	}

	inline void RenderMesh(VertexArray& vertexArray, const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(&vertexArray, nullptr)].push_back(perspective * mainCamera->GetViewMatrix() * transformIn);
	}

	inline void RenderPrimitives(VertexArray* vertexArray, Shader* InShader, mat4 transform, DrawParams drawParamsIn)
	{
		Array<mat4> transforms;
		transforms.push_back(transform);
		vertexArray->UpdateBuffer(4, &transforms[0], transforms.size() * sizeof(mat4));
		Draw(*InShader, *vertexArray, drawParamsIn, 1);

	}

	inline void RenderDebugShapes(DebugShape* shapeIn, const mat4& transformIn)
	{
		debugShapeBuffer[shapeIn].push_back(perspective * mainCamera->GetViewMatrix() * transformIn);
	}

	void DrawEditorHelpers();
	void DrawDebugShapes();
	
    void Flush();
    //~EditorRenderContext();

	
private:
	DrawParams& drawParams;
	//Shader& shader;
	Sampler& sampler;
	mat4 perspective;
	Map<std::pair<VertexArray*, Texture*>, Array<mat4>> meshRenderBuffer;
	Map<DebugShape*, Array<mat4>> debugShapeBuffer;

	Camera* mainCamera;

	//Grid VO
	Transform editorGridTransform;
	VertexArray* editorGridVA;
	DrawParams editorGridDrawParams;
	float editorGridSlices;
	float editorGridScale;
};

