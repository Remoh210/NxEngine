#pragma once
#include "common/Common.h"
#include "RenderDevice.h"
#include "IndexedModel.h"

class VertexArray
{
public:
	inline VertexArray(RenderDevice& deviceIn, const IndexedModel& model,
			BufferUsage usage) :
		device(&deviceIn),
		id(model.CreateVertexArray(deviceIn, usage)),
		numIndices(model.GetNumIndices()) 
		{}

	inline ~VertexArray()
	{
		//id = device->ReleaseVertexArray(id);
	}

	inline void UpdateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		return device->UpdateVertexArrayBuffer(id, bufferIndex, data, dataSize);
	}

	inline uint32 GetId() { return id; };
	inline uint32 GetNumIndices(){ return numIndices; };
private:
	RenderDevice* device;
	uint32 id;
	uint32 numIndices;

	//NULL_COPY_AND_ASSIGN(VertexArray);
};

