#pragma once
#include "common/Common.h"
#include "RenderDevice.h"
#include "IndexedModel.h"

class VertexArray
{
public:
    VertexArray(RenderDevice& deviceIn, const IndexedModel& model);
    
    inline uint32 GetId() {return Id;}
    inline uint32 GetNumIndices() {return NumIndices;}
    inline void UpdateBuffer(uint32 bufferIndex,
		const void* data, uintptr dataSize)
	{
	//return Device->updateVertexArrayBuffer(Id, bufferIndex, data, dataSize);
	}

private:
	RenderDevice* Device;
	uint32 Id;
	uint32 NumIndices;
      
};
