#pragma once
#include "Common/Common.h"
#include "RenderDevice.h"


class IndexedModel
{
public:
	IndexedModel() :
		instancedElementsStartIndex((uint32)-1) {}
	uint32 CreateVertexArray(RenderDevice* device,
			BufferUsage usage) const;

	void AllocateElement(uint32 elementSize);
	void SetInstancedElementStartIndex(uint32 elementIndex);

	void AddElement1f(uint32 elementIndex, float e0);
	void AddElement2f(uint32 elementIndex, float e0, float e1);
	void AddElement3f(uint32 elementIndex, float e0, float e1, float e2);
	void AddElement4f(uint32 elementIndex, float e0, float e1, float e2, float e3);

	void AddIndices1i(uint32 i0);
	void AddIndices2i(uint32 i0, uint32 i1);
	void AddIndices3i(uint32 i0, uint32 i1, uint32 i2);
	void AddIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3);

	uint32 GetNumIndices() const;
private:
	Array<uint32> indices;
	Array<uint32> elementSizes;
	Array<Array<float> > elements;
	uint32 instancedElementsStartIndex;
};
