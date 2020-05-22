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
    
    inline void clear(bool shouldClearColor, bool shouldClearDepth,
			bool shouldClearStencil, const vec3& color, uint32 stencil)
            
    {
        //device->clear
    }
	inline void clear(const vec3& color, bool shouldClearDepth=false)
    {
        //device->clear()
    }
	inline void draw(Shader& shader, VertexArray& vertexArray, 
			const DrawParams& drawParams, uint32 numInstances=1)
    {
        //
    }
	inline void draw(Shader& shader, VertexArray& vertexArray, 
			const DrawParams& drawParams, uint32 numInstances,
			uint32 numIndices)
    {
        //
    }

    ~RenderContext();
private:
    RenderDevice* mRenderDevice;
    RenderTarget* mRenderTarget;
};
