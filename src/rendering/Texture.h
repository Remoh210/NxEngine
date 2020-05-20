#pragma once
#include "common/Common.h"
#include "ArrayBitmap.h"
#include "RenderDevice.h"

class Texture
{

public:
    inline Texture(RenderDevice& deviceIn, const ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps, bool bCompress)
            :device(&deviceIn),
            width(texData.GetWidth()), height(texData.GetHeight()),
            bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps), 
            id(deviceIn.CreateTexture2D(width, height, PixelFormat::FORMAT_RGBA,
            texData.GetData(), interalPixelFormat, bCompress, bGenerateMipMaps))
            {}

    ~Texture();

private:
    RenderDevice* device;
    uint32 id;
	uint32 width;
	uint32 height;
	bool bIsCompressed;
	bool bHasMipmaps;
};


