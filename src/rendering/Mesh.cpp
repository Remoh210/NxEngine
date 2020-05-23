#include "Mesh.h"


Mesh::Mesh(Array<sVertex> vertices, Array<uint32> indices) 
{
    this->mVertices = vertices;
	this->mIndices = indices;
    
}
