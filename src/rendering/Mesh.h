#include "common/dataStructs/Array.h"
#include "common/Common.h"
#include "common/CommonTypes.h"

struct sVertex {
	// position
	vec3 Position;
	// normal
	vec3 Normal;
	// texCoords
	vec2 TexCoords;
	// tangent
	vec3 Tangent;
	// bitangent
	vec3 Bitangent;
};

class Mesh
{
public:
    Mesh(Array<sVertex> vertices, Array<uint32> indices);
    inline Array<sVertex> GetVeteries() const { return mVertices; };
	inline Array<uint32> GetIndices() const { return mIndices; } ;
    ~Mesh();

private:
    Array<sVertex> mVertices;
	Array<uint32> mIndices;

    uint32 VAO;
    uint32 EBO;
};

