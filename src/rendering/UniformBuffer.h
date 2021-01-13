#pragma once

#include "RenderDevice.h"

class UniformBuffer
{
public:

	UniformBuffer(RenderDevice* deviceIn, enum BufferUsage usageIn);


	void AllocateElement(uintptr dataSize);

	inline void Generate()
	{
		UniformId = device->CreateUniformBuffer(nullptr, size, usage);
	}

	inline ~UniformBuffer()
	{
		UniformId = device->ReleaseUniformBuffer(UniformId);
	}

	void Update(const void* data, uintptr dataSize, uintptr base);

	inline void ResetOffset()
	{
		offset = 0;
	}

	inline uint32 GetId() { return UniformId; }
private:
	RenderDevice* device;
	uint32 UniformId;

	BufferUsage usage;
	uintptr padding;
	uintptr offset;
	uintptr size;
	uintptr availableMemory;
};