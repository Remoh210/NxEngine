#pragma once
#include "Common/Common.h"
#include "RenderDevice.h"

struct NxVertex
{
	NxVertex() :
		x(0.0f), y(0.0f), z(0.0f), w(1.0f),
		r(0.0f), g(0.0f), b(0.0f), a(1.0f),		// Note alpha is 1.0
		nx(0.0f), ny(0.0f), nz(0.0f), nw(1.0f),
		u0(0.0f), v0(0.0f), u1(0.0f), v1(0.0f),
		tx(0.0f), ty(0.0f), tz(0.0f), tw(1.0f),
		bx(0.0f), by(0.0f), bz(0.0f), bw(1.0f)
	{
	};


	NxVertex(float vertex_x, float vertex_y, float vertex_z) :
		x(vertex_x), y(vertex_y), z(vertex_z), w(1.0f),
		r(0.0f), g(0.0f), b(0.0f), a(1.0f),		// Note alpha is 1.0
		nx(0.0f), ny(0.0f), nz(0.0f), nw(1.0f),
		u0(0.0f), v0(0.0f), u1(0.0f), v1(0.0f),
		tx(0.0f), ty(0.0f), tz(0.0f), tw(1.0f),
		bx(0.0f), by(0.0f), bz(0.0f), bw(1.0f)
	{
	};

	float x, y, z, w;								// 16
	float r, g, b, a;								// 32 
	float nx, ny, nz, nw;							// 48 
	float u0, v0, u1, v1;							// 60
	float tx, ty, tz, tw;	// tangent				// 
	float bx, by, bz, bw;	// bi-normal			// 
};

struct NxTriangle
{
	NxTriangle() : vertex_ID_0(0), vertex_ID_1(0), vertex_ID_2(0) {};
	~NxTriangle() {};
	int vertex_ID_0;
	int vertex_ID_1;
	int vertex_ID_2;
};


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
	NxArray<uint32> IndexedModel::GetIndices() const;
	NxArray<float> IndexedModel::GetElement(uint32 elementNumber) const;

	struct AABB
	{
		float max[3];
		float min[3];
	};

	AABB aabb;

	NxArray<NxVertex> vertices;
	NxArray<NxTriangle> triangles;

	//NxArray<VertexData> vertexData;
private:
	NxArray<uint32> indices;
	NxArray<uint32> elementSizes;
	NxArray<NxArray<float>> elements;
	uint32 instancedElementsStartIndex;

	
};
