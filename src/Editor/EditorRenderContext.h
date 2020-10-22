#pragma once

#include "rendering/RenderDevice.h"
#include "rendering/RenderContext.h"
#include "rendering/Shader.h"
#include "rendering/Sampler.h"
#include "rendering/VertexArray.h"
#include "Common/CommonTypes.h"
#include "Common/dataStructs/Map.h"
#include "Core/Graphics/LineRenderer/LineRenderer.h"
#include <Core/Camera/Camera.h>
#include <Core/Graphics/LineRenderer/LineRenderer.h>

class EditorRenderContext : public RenderContext
{
public:
	EditorRenderContext(RenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn,
	        Sampler& samplerIn, const mat4& perspectiveIn, Camera* CameraIn);
	inline void RenderMesh(VertexArray& vertexArray, Texture& texture, const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(&vertexArray, &texture)].push_back(perspective * mainCamera->GetViewMatrix() * transformIn);
	}

	inline void RenderPrimitives(VertexArray* vertexArray, Shader* InShader, Texture* texture, uint32 numVertecies, mat4 transform)
	{
		Array<mat4> transforms;
		transforms.push_back(transform);
		vertexArray->UpdateBuffer(4, &transforms[0], transforms.size() * sizeof(mat4));
		mRenderDevice->DrawArrays(mRenderTarget->GetId(), vertexArray->GetShader()->GetId(), vertexArray->GetId(),
			drawParams, numVertecies);

	}

	void DrawEditorHelpers();
	
    void Flush();
    //~EditorRenderContext();

	
private:
	DrawParams& drawParams;
	//Shader& shader;
	Sampler& sampler;
	mat4 perspective;
	Map<std::pair<VertexArray*, Texture*>, Array<mat4>> meshRenderBuffer;

	Camera* mainCamera;

	//Grid VO
	VertexArray* EditorGrid;
	DrawParams GridDrawParams;
};

