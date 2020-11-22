#pragma once

#include "RenderDevice.h"

class Sampler
{
public:
	inline Sampler(RenderDevice* deviceIn,
			SamplerFilter minFilter
				= SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR,
			SamplerFilter magFilter = SamplerFilter::FILTER_LINEAR,
			SamplerWrapMode wrapU = SamplerWrapMode::WRAP_CLAMP,
			SamplerWrapMode wrapV = SamplerWrapMode::WRAP_CLAMP,
			float anisotropy = 0.0f) :
		device(deviceIn),
		id(device->CreateSampler(minFilter, magFilter,
				wrapU, wrapV, anisotropy)) {}
	inline ~Sampler()
	{
		id = device->ReleaseSampler(id);
	}

	inline uint32 GetId(){ return id; };
private:
	RenderDevice* device;
	uint32 id;

	//NULL_COPY_AND_ASSIGN(Sampler);
};



