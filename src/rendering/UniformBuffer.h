#pragma once

#include "RenderDevice.h"

class UniformBuffer
{
public:
	inline UniformBuffer(RenderDevice& deviceIn, uint32 numder, uintptr dataSize,
		enum BufferUsage usage, const void* data = nullptr) :
		device(&deviceIn),
		UniformId(device->CreateUniformBuffer(data, numder * dataSize, usage)),
		size(dataSize) 
	{
		padding = dataSize;
		//Offset = 0;
	}

	inline ~UniformBuffer()
	{
		UniformId = device->ReleaseUniformBuffer(UniformId);
	}

	inline void Update(const void* data, uintptr dataSize, uint32 offset)
	{
		device->UpdateUniformBuffer(UniformId, data, dataSize, offset * dataSize);
		//Offset += dataSize;
	}
	inline void Update(const void* data) 
	{
		//Update(data, size);
	}

	inline void ResetOffset()
	{
		Offset = 0;
	}

	inline uint32 GetId() { return UniformId; }
private:
	RenderDevice* device;
	uint32 UniformId;
	uintptr Offset;

	uintptr padding;
	uintptr size;
};