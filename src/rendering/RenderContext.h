#pragma once

#include "RenderDevice.h"
#include "RenderTarget.h"
#include "common/CommonTypes.h"
#include "Shader.h"
#include "VertexArray.h"

class RenderContext
{

public:
    RenderContext(RenderDevice& deviceIn, RenderTarget& targetIn):
            mRenderDevice(&deviceIn), mRenderTarget(&targetIn)
            {}
    
    inline void Clear(bool bShouldClearColor, bool bShouldClearDepth,
			bool bShouldClearStencil, const vec4& color, uint32 stencil)
            
    {
        	mRenderDevice->Clear(mRenderTarget->GetId(), bShouldClearColor, bShouldClearDepth, bShouldClearStencil,
			color, stencil);
    }
	inline void Clear(const vec4& color, bool bShouldClearDepth=false)
    {
        mRenderDevice->Clear(mRenderTarget->GetId(), true, bShouldClearDepth, false, color, 0);
    }
	inline void Draw(Shader& shader, VertexArray& vertexArray,
			const DrawParams& drawParams, uint32 numInstances=1)
    {
        mRenderDevice->Draw(mRenderTarget->GetId(), shader.GetId(), vertexArray.GetId(),
                     drawParams, numInstances, vertexArray.GetNumIndices());
    }
	inline void Draw(Shader& shader, VertexArray& vertexArray,
			const DrawParams& drawParams, uint32 numInstances,
			uint32 numIndices)
    {
        mRenderDevice->Draw(mRenderTarget->GetId(), shader.GetId(), vertexArray.GetId(),
                drawParams, numInstances, numIndices);
    }

    ~RenderContext(){};
protected:
    RenderDevice* mRenderDevice;
    RenderTarget* mRenderTarget;
};
