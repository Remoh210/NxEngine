#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(RenderDevice* deviceIn, enum BufferUsage usageIn)
	:device(deviceIn)
	,usage(usageIn)
{
	availableMemory = 0;
	offset = 0;
	size = 0;

}

void UniformBuffer::AllocateElement(uintptr dataSize)
{
	

	if (dataSize < 16)
	{
		if (availableMemory == 0)
		{
			availableMemory = 16;
			size += availableMemory;
			availableMemory -= dataSize;
		}
		else if (availableMemory > dataSize)
		{
			availableMemory -= dataSize;
		}
		else if (availableMemory < dataSize)
		{
			//
		}
			
	}
	else
	{
		size += dataSize;
	}
}

void UniformBuffer::Update(const void* data, uintptr dataSize, uintptr base)
{
	if (offset % base == 0)
	{
		device->UpdateUniformBuffer(UniformId, data, dataSize, offset);
		offset += dataSize;
	}
	else
	{
		while (offset % base != 0)
		{
			offset += 1;
		}

		device->UpdateUniformBuffer(UniformId, data, dataSize, offset);

	}

}