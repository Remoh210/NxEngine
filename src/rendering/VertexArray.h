#pragma once
#include "common/Common.h"
#include "RenderDevice.h"
#include "IndexedModel.h"
#include  "rendering/Shader.h"

class VertexArray
{
public:
	inline VertexArray(RenderDevice& deviceIn, const IndexedModel& model,
			BufferUsage usage, Shader* inShader) :
		device(&deviceIn),
		id(model.CreateVertexArray(deviceIn, usage)),
		numIndices(model.GetNumIndices()),
		shader(inShader)
		{}

	inline VertexArray(RenderDevice& deviceIn, uint32 idIn,
		BufferUsage usage, uint32 numVertecies) :
		device(&deviceIn),
		id(idIn),
		numIndices(numVertecies)
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
	Shader* shader;
private:
	RenderDevice* device;
	uint32 id;
	uint32 numIndices;

	//NULL_COPY_AND_ASSIGN(VertexArray);
};

