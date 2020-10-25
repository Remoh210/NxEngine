#pragma once

#include "RenderDevice.h"

class UniformBuffer
{
public:
	inline UniformBuffer(RenderDevice& deviceIn, uintptr dataSize,
		enum BufferUsage usage, const void* data = nullptr) :
		device(&deviceIn),
		UniformId(device->CreateUniformBuffer(data, dataSize, usage)),
		size(dataSize) 
	{
		Offset = 0;
	}

	inline ~UniformBuffer()
	{
		UniformId = device->ReleaseUniformBuffer(UniformId);
	}

	inline void Update(const void* data, uintptr dataSize)
	{
		device->UpdateUniformBuffer(UniformId, data, dataSize, Offset);
		Offset += dataSize;
	}
	inline void Update(const void* data) 
	{
		Update(data, size);
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
	uintptr size;
};