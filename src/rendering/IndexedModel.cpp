#include "IndexedModel.h"

void IndexedModel::AddElement1f(uint32 elementIndex, float e0)
{
	elements[elementIndex].push_back(e0);
}

void IndexedModel::AddElement2f(uint32 elementIndex, float e0, float e1)
{
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
}

void IndexedModel::AddElement3f(uint32 elementIndex, float e0, float e1, float e2)
{
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
}

void IndexedModel::AddElement4f(uint32 elementIndex, float e0, float e1, float e2, float e3)
{
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
	elements[elementIndex].push_back(e3);
}

void IndexedModel::AddIndices1i(uint32 i0)
{
	indices.push_back(i0);
}

void IndexedModel::AddIndices2i(uint32 i0, uint32 i1)
{
	indices.push_back(i0);
	indices.push_back(i1);
}

void IndexedModel::AddIndices3i(uint32 i0, uint32 i1, uint32 i2)
{
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
}

void IndexedModel::AddIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3)
{
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

uint32 IndexedModel::GetNumIndices() const
{
	return indices.size();
}

std::vector<uint32> IndexedModel::GetIndices() const
{
	return indices;
}

std::vector<float> IndexedModel::GetElement(uint32 elementNumber) const
{
	return elements[elementNumber];
}

void IndexedModel::AllocateElement(uint32 elementSize)
{
	elementSizes.push_back(elementSize);
	elements.push_back(NxArray<float>());
}

void IndexedModel::SetInstancedElementStartIndex(uint32 elementIndex)
{
	instancedElementsStartIndex = elementIndex;
}

//void IndexedModel::clear()
//{
//	instancedElementsStartIndex = -1;
//	indices.clear();
//	elementSizes.clear();
//	elements.clear();
//}

uint32 IndexedModel::CreateVertexArray(RenderDevice* device,
	enum BufferUsage usage) const
{
	uint32 numVertexComponents = elementSizes.size();
	uint32 numInstanceComponents = instancedElementsStartIndex == ((uint32)-1) ?
		0 : (numVertexComponents - instancedElementsStartIndex);
	numVertexComponents -= numInstanceComponents;

	NxArray<const float*> vertexDataNxArray;
	for (uint32 i = 0; i < numVertexComponents; i++) {
		vertexDataNxArray.push_back(&(elements[i][0]));
	}

	const float** vertexData = &vertexDataNxArray[0];
	const uint32* vertexElementSizes = &elementSizes[0];

	uint32 numVertices = elements[0].size() / vertexElementSizes[0];
	uint32 numIndices = indices.size();

	if (indices.size() == 0)
	{
		return device->CreateVertexArray(vertexData, vertexElementSizes,
			numVertexComponents, numInstanceComponents, numVertices, nullptr,
			numIndices, usage);
	}

	return device->CreateVertexArray(vertexData, vertexElementSizes,
		numVertexComponents, numInstanceComponents, numVertices, &indices[0],
		numIndices, usage);
}

