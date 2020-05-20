#pragma once
#include "common/Common.h"
#include "ArrayBitmap.h"
#include "RenderDevice.h"

class Texture
{

public:
    inline Texture(RenderDevice& deviceIn, ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps = true, bool bCompress = true)
            :device(&deviceIn),
            width(texData.GetWidth()), height(texData.GetHeight()),
            bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps), 
            id(deviceIn.CreateTexture2D(texData.GetWidth(), texData.GetHeight(), PixelFormat::FORMAT_RGBA,
            texData.GetData(), interalPixelFormat, bCompress, bGenerateMipMaps))
    {
		texData.Clean();
	}
    inline uint32 GetId() { return id; };

    ~Texture() {};

private:
    RenderDevice* device;
    uint32 id;
	uint32 width;
	uint32 height;
	bool bIsCompressed;
	bool bHasMipmaps;
};


