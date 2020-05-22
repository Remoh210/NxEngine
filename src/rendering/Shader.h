#include "RenderDevice.h"
#include "Texture.h"

class Shader
{
public:
    Shader(RenderDevice& deviceIn, const String& shaderText):
            mRenderDevice(&deviceIn),
            mProgramId(deviceIn.CreateShaderProgram(shaderText))
            {}
    inline uint32 GetId() { return mProgramId; };
    ~Shader();
    inline ~Shader()
	{
		//mRenderDevice->ReleaseShaderProgram(deviceId);
	}

	//inline void setUniformBuffer(const String& name, UniformBuffer& buffer)
    //{
    //    
    //}
	//inline void setSampler(const String& name, Texture& texture, Sampler& sampler,
	//		uint32 unit);

private:
    RenderDevice* mRenderDevice;
    uint32 mProgramId;
};

