#pragma once 

#include "common/Common.h"
#include "common/dataStructs/String.h"

class Texture
{
public:
    Texture(const string& fileName);

    inline uint8* GetData() { return data; };
    ~Texture();
private:
    bool Load(const string& fileName);
    uint8* data;
};

