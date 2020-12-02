#include "Cubemap.h"

Cubemap::Cubemap(RenderDevice* deviceIn, uint32 widthIn, uint32 heightIn)
	:width(widthIn)
	,height(heightIn)
	,device(deviceIn)
	,id(device->CreateTextureCube(widthIn, heightIn, PixelFormat::FORMAT_RGB16F, nullptr, PixelFormat::FORMAT_RGB, false, false))
{

}

