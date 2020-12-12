#include "Cubemap.h"

Cubemap::Cubemap(RenderDevice* deviceIn, uint32 widthIn, uint32 heightIn, bool bGenerateMipmaps)
	:width(widthIn)
	,height(heightIn)
	,device(deviceIn)
	,id(device->CreateTextureCube(widthIn, heightIn, PixelFormat::FORMAT_RGB, nullptr, PixelFormat::FORMAT_RGB16F, bGenerateMipmaps, false))
{

}

