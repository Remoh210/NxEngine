#include "RenderDevice.h"
#include "RenderTarget.h"
#include "common/CommonTypes.h"
#include "Shader.h"
#include "VertexArray.h"

class RenderContext
{

public:
    RenderContext(RenderDevice& deviceIn, RenderTarget& targetIn):
            mRenderDevice(&deviceIn), mRenderTarget(mRenderTarget)
            {}
    
    inline void Clear(bool shouldClearColor, bool shouldClearDepth,
			bool shouldClearStencil, const vec3& color, uint32 stencil)
            
    {
        //device->clear
    }
	inline void Clear(const vec3& color, bool shouldClearDepth=false)
    {
        //device->clear()
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
private:
    RenderDevice* mRenderDevice;
    RenderTarget* mRenderTarget;
};
