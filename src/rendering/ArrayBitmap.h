#pragma once 

#include "Common/Common.h"
#include "Common/dataStructs/String.h"
#include "RenderDevice.h"

class ArrayBitmap
{
public:
    ArrayBitmap(/*const string& fileName*/);
	bool Load(const NString& fileName, bool bFloat = false);

    inline void* GetData() const { return data; };
	inline float* GetDataf() const { return fData; };
    inline int32 GetWidth() const { return width; };
    inline int32 GetHeight() const { return height; };
    inline NString GetFileName() const { return texturefile; };
	inline PixelFormat GetFormat() const { return format; };
	void Clean();
    ~ArrayBitmap();
private:
    NString texturefile;
    void* data;
	float* fData;
    int32 width;
    int32 height;
	PixelFormat format;
};

