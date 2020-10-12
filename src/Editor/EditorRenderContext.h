#pragma once

#include "rendering/RenderDevice.h"
#include "rendering/RenderContext.h"
#include "rendering/Shader.h"
#include "rendering/Sampler.h"
#include "rendering/VertexArray.h"
#include "common/CommonTypes.h"
#include "common/dataStructs/Map.h"

class EditorRenderContext : public RenderContext
{

public:
	EditorRenderContext(RenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn,
			Shader& shaderIn, Sampler& samplerIn, const mat4& perspectiveIn) 
            : RenderContext(deviceIn, targetIn),
		    drawParams(drawParamsIn), shader(shaderIn), sampler(samplerIn), perspective(perspectiveIn) 
        {}
	inline void RenderMesh(VertexArray& vertexArray, Texture& texture, const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(&vertexArray, &texture)].push_back(perspective * transformIn);
	}
	
    void Flush();
    //~EditorRenderContext();

private:
	DrawParams& drawParams;
	Shader& shader;
	Sampler& sampler;
	mat4 perspective;
	Map<std::pair<VertexArray*, Texture*>, Array<mat4>> meshRenderBuffer;
};

