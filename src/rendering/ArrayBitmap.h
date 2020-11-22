#pragma once 

#include "Common/Common.h"
#include "Common/dataStructs/String.h"
#include "RenderDevice.h"

class ArrayBitmap
{
public:
    ArrayBitmap(/*const string& fileName*/);
	bool Load(const NString& fileName);

    inline uint8* GetData() const { return data; };
    inline int32 GetWidth() const { return width; };
    inline int32 GetHeight() const { return height; };
    inline NString GetFileName() const { return texturefile; };
	inline PixelFormat GetFormat() const { return format; };
	void Clean();
    ~ArrayBitmap();
private:
    NString texturefile;
    uint8* data;
    int32 width;
    int32 height;
	PixelFormat format;
};

