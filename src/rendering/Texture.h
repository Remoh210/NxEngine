#pragma once
#include "common/Common.h"
#include "ArrayBitmap.h"
#include "RenderDevice.h"

class Texture
{

public:
    inline Texture(RenderDevice& deviceIn, const ArrayBitmap& texData, PixelFormat interalPixelFormat,
            bool bGenerateMipMaps, bool bCompress);
    ~Texture();

private:
    /* data */
};


