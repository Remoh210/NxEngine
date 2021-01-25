#pragma once
#include "RenderDevice.h"
#include "Core/Graphics/Texture/Texture.h"
#include "Core/Graphics/Cubemap/Cubemap.h"
#include "UniformBuffer.h"
#include "Sampler.h"

class Shader
{
public:
    Shader(RenderDevice* deviceIn, const NString& shaderText):
            mRenderDevice(deviceIn),
            mProgramId(deviceIn->CreateShaderProgram(shaderText))
            {}
    inline uint32 GetId() { return mProgramId; };
    inline ~Shader()
	{
		//mRenderDevice->ReleaseShaderProgram(deviceId);
	}

	inline void SetUniformBuffer(const NString& name, UniformBuffer& buffer)
	{
		mRenderDevice->SetShaderUniformBuffer(mProgramId, name, buffer.GetId());
	}
	inline void SetUniform1i(const NString& name, int value)
	{
		mRenderDevice->SetShaderUniform1i(mProgramId, name, value);
	}
	inline void SetUniform1iv(const NString& name, NxArray<int> value)
	{
		mRenderDevice->SetNxArrayUniform1iv(mProgramId, name, value.size(), &value[0]);
	}
	inline void SetUniform1f(const NString& name, float value)
	{
		mRenderDevice->SetShaderUniform1f(mProgramId, name, value);
	}
	inline void SetUniform3f(const NString& name, vec3 value)
	{
		mRenderDevice->SetShaderUniform3f(mProgramId, name, glm::value_ptr(value));
	}
	inline void SetUniform2f(const NString& name, vec2 value)
	{
		mRenderDevice->SetShaderUniform2f(mProgramId, name, glm::value_ptr(value));
	}
	inline void SetUniform4f(const NString& name, vec4 value)
	{
		mRenderDevice->SetShaderUniform4f(mProgramId, name, glm::value_ptr(value));
	}
    inline void SetSampler(const NString& name, Texture& texture, Sampler& sampler,
            uint32 unit)
    {
        mRenderDevice->SetShaderSampler(mProgramId, name, texture.GetId(), sampler.GetId(), unit);
    }
	inline void SetSampler3D(const NString& name, Cubemap& texture, Sampler& sampler,
		uint32 unit)
	{
		mRenderDevice->SetShaderSampler3D(mProgramId, name, texture.GetId(), sampler.GetId(), unit);
	}
	inline void SetUniform3fv(const NString& name, NxArray<vec3> value)
	{
		mRenderDevice->SetNxArrayUniform3fv(mProgramId, name, value.size(), glm::value_ptr(value[0]));
	}
	inline void SetUniform4fv(const NString& name, NxArray<vec4> value)
	{
		mRenderDevice->SetShaderUniform4fv(mProgramId, name, value.size(), glm::value_ptr(value[0]));
	}
	inline void SetUniformMat4(const NString& name, mat4 value)//?? HOW
	{
		mRenderDevice->SetShaderUniformMat4(mProgramId, name, glm::value_ptr(value[0]));//??
	}

	inline void SetUniformMat4v(const NString& name, NxArray<mat4> value)//?? HOW
	{
		mRenderDevice->SetShaderUniformMat4v(mProgramId, name, value.size(), glm::value_ptr(value[0]));//??
	}

private:
    RenderDevice* mRenderDevice;
    uint32 mProgramId;
};

