#pragma once
#include "Common/Common.h"
#include "ArrayBitmap.h"
#include "RenderDevice.h"
#include "Common/dataStructs/String.h"

enum TextureType
{
    Diffuse,
    Normal,
    Alpha
};

class Texture
{

public:
    inline Texture(RenderDevice* deviceIn, ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps = true, bool bCompress = true)
            :device(deviceIn),
            width(texData.GetWidth()), height(texData.GetHeight()),
            bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps), 
            fileName(texData.GetFileName()),
            id(deviceIn->CreateTexture2D(texData.GetWidth(), texData.GetHeight(), texData.GetFormat(),
            texData.GetData(), interalPixelFormat, bGenerateMipMaps, bCompress))
    {
		texData.Clean();
	}

	inline Texture(uint32 id)
		:id(id)
	{
	}

    inline uint32 GetId() { return id; };
    inline NString GetFileName() {return fileName; };
    inline uint32 GetWidth() { return width; };
    inline uint32 GetHeight() { return height; };
    inline bool IsCompressed() { return bIsCompressed; };
    inline bool HasMipmaps() { return bHasMipmaps; };

    ~Texture() 
    {
        device->ReleaseTexture2D(id);
    };

private:
    RenderDevice* device;
    uint32 id;
	uint32 width;
	uint32 height;
    NString fileName;
	bool bIsCompressed;
	bool bHasMipmaps;
};


