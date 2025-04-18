#pragma once
#include "Common/Common.h"
#include "rendering/ArrayBitmap.h"
#include "rendering/RenderDevice.h"
#include "Common/dataStructs/String.h"

#include "rttr/registration.h"

enum TextureType
{
    Diffuse,
    Normal,
    Alpha
};

class Texture
{

public:
	inline Texture()
	{
		
	}
	
    inline Texture(RenderDevice* deviceIn, ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps = true, bool bCompress = true, bool bFloatType = false)
            :device(deviceIn),
            width(texData.GetWidth()), height(texData.GetHeight()),
            bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps), 
            fileName(texData.GetFileName())
    {
		id = deviceIn->CreateTexture2D(texData.GetWidth(), texData.GetHeight(), texData.GetFormat(),
			texData.GetData(), interalPixelFormat, bGenerateMipMaps, bCompress, bFloatType);
		texData.Clean();
	}

	inline Texture(RenderDevice* deviceIn, uint32 widthIn,
		uint32 heightIn, PixelFormat pixelFormat, PixelFormat internalPixelFormat, bool bGenerateMipMaps = true, bool bCompress = true, bool bFloatType = false)
		:device(deviceIn),
		width(widthIn), height(heightIn),
		bIsCompressed(bCompress), bHasMipmaps(bGenerateMipMaps),
		fileName("")
	{
		id = deviceIn->CreateTexture2D(widthIn, heightIn, pixelFormat,
			nullptr, internalPixelFormat, bGenerateMipMaps, bCompress, bFloatType);
	}

	inline Texture(uint32 id)
		:id(id)
	{
	}


    inline uint32& GetId() { return id; };
	inline void SetId(uint32& idIn) { id = idIn; };
    inline NString& GetFileName() {return fileName; };
	inline void SetFileName(NString& nameIn) {fileName = nameIn; };
    inline uint32 GetWidth() { return width; };
    inline uint32 GetHeight() { return height; };
    inline bool IsCompressed() { return bIsCompressed; };
    inline bool HasMipmaps() { return bHasMipmaps; };

    virtual ~Texture() 
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


	RTTR_ENABLE();
};


