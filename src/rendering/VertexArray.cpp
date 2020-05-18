#include "VertexArray.h"


VertexArray::VertexArray(RenderDevice& deviceIn, const IndexedModel& model)
                        :Device(&deviceIn)
                        ,Id(model.createVertexArray(deviceIn))
{ 
    
}
