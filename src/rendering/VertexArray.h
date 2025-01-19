#pragma once
#include "Common/Common.h"
#include "RenderDevice.h"
#include "IndexedModel.h"
#include "rendering/Shader.h"
#include "Common/dataStructs/NxArray.h"

class VertexArray
{
public:
	inline VertexArray(RenderDevice* deviceIn, const IndexedModel& modelIn,
			BufferUsage usage) :
		device(deviceIn),
		model(modelIn),
		id(modelIn.CreateVertexArray(deviceIn, usage)),
		numIndices(model.GetNumIndices()),
		numVertices(model.GetElement(0).size())
		{}


	inline VertexArray(RenderDevice& deviceIn, uint32 idIn,
		BufferUsage usage, uint32 numVertecies) :
		device(&deviceIn),
		id(idIn),
		numIndices(numVertecies)
	{}

	inline ~VertexArray()
	{
		device->ReleaseVertexArray(id);
	}

	inline void UpdateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		return device->UpdateVertexArrayBuffer(id, bufferIndex, data, dataSize);
	}

    inline void SetShader(Shader* shaderIn)
    {
        shader = shaderIn;
    }

    inline Shader* GetShader()
    {
        return shader;
    }

	inline uint32 GetId() { return id; };
	inline IndexedModel GetIndexedModel() const { return model; };
	inline uint32 GetNumIndices() { return numIndices; };
	inline uint32 GetNumVertices() { return numVertices; };

private:
	RenderDevice* device;
	IndexedModel model;
    Shader* shader;
	uint32 id;
	uint32 numIndices;
	uint32 numVertices;

	//NULL_COPY_AND_ASSIGN(VertexArray);
};

