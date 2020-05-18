#pragma once
#include "common/Common.h"
#include "RenderDevice.h"

class IndexedModel
{
private:

public:
    IndexedModel();
    uint32 createVertexArray(RenderDevice& device) const;
    ~IndexedModel();
};

