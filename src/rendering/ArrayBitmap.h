#pragma once 

#include "common/Common.h"
#include "common/dataStructs/String.h"

class ArrayBitmap
{
public:
    ArrayBitmap(/*const string& fileName*/);
	bool Load(const string& fileName);

    inline uint8* GetData() const { return data; };
    inline int32 GetWidth() const { return width; };
    inline int32 GetHeight() const { return height; };
	void Clean();
    ~ArrayBitmap();
private:
    
    uint8* data;
    int32 width;
    int32 height;
};

