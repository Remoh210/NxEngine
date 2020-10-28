#pragma once
#include "Common/Common.h"
#include "ArrayBitmap.h"
#include "RenderDevice.h"

enum TextureType
{
    Diffuse,
    Normal,
    Alpha
};

class Texture
{

public:
    inline Texture(RenderDevice& deviceIn, ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps = true, bool bCompress = true)
            :device(&deviceIn),
            width(texData.GetWidth()), height(texData.GetHeight()),
            bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps), 
            id(deviceIn.CreateTexture2D(texData.GetWidth(), texData.GetHeight(), texData.GetFormat(),
            texData.GetData(), interalPixelFormat, bGenerateMipMaps, bCompress))
    {
		texData.Clean();
	}
    inline uint32 GetId() { return id; };
    inline uint32 GetWidth() { return width; };
    inline uint32 GetHeight() { return height; };

    ~Texture() {};

private:
    RenderDevice* device;
    uint32 id;
	uint32 width;
	uint32 height;
	bool bIsCompressed;
	bool bHasMipmaps;
};


