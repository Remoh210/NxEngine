#pragma once 

#include "rendering/ArrayBitmap.h"
#include "rendering/RenderDevice.h"

class Cubemap
{
public:
	Cubemap(RenderDevice* deviceIn, uint32 widthIn, uint32 heightIn);
	~Cubemap() { device->ReleaseTexture2D(id); }

	inline uint32 GetId() { return id; }
	inline uint32 GetWidth() { return width; }
	inline uint32 GetHeight() { return height; }


private:
	uint32 id;
	bool bIsHDR;
	uint32 width;
	uint32 height;
	RenderDevice* device;
};
