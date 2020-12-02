#pragma once
#include <Common/Common.h>
#include <Common/CommonTypes.h>
#include <Common/dataStructs/Array.h>
#include <rendering/RenderDevice.h>
#include <iostream>

#define PI 3.14

class PrimitiveGenerator
{
public:
	PrimitiveGenerator() {}

	~PrimitiveGenerator() {}


	inline static IndexedModel CreateGridVA(uint32 slices, vec3 color = vec3(1.0f, 0.0f, 0.0f))
	{
		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Tangents
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix
        
		glm::vec3 extents(0.0f);

        //Array<vec3> vertices;
        for(uint32 j=0; j<=slices; ++j)
        {
          for(uint32 i=0; i<=slices; ++i)
          {
            float x = (float)i/(float)slices;
			if (x > extents.x) { extents.x = x; }
            float y = 0;
            float z = (float)j/(float)slices;
			if (z > extents.z) { extents.z = z; }
            newModel.AddElement3f(0, x, y, z);
            newModel.AddElement2f(1, 0, 0);
            newModel.AddElement3f(2, color.x, color.y, color.z);
            newModel.AddElement3f(3, 0, 0, 0);
          }
        }
        
		DEBUG_LOG_TEMP("Extents: x: %f, y: %f, z: %f", extents.x, extents.y, extents.z);

        for(uint32 j=0; j<slices; ++j)
        {
          for(uint32 i=0; i<slices; ++i)
          {

            uint32 row1 =  j    * (slices+1);
			uint32 row2 = (j+1) * (slices+1);
              
            newModel.AddIndices4i(row1+i, row1+i+1, row1+i+1,row2+i+1);
            newModel.AddIndices4i(row2+i+1, row2+i, row2+i, row1+i);
            
          }
        }

		return newModel;
	}

	inline static IndexedModel CreateSphere(float radius, uint32 sectorCount, uint32 stackCount, vec3 color)
	{
		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Color
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix

		//songho.ca/opengl/gl_sphere.html
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		for(uint32 i = 0; i <= stackCount; ++i)
		{
		    stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		    xy = radius * cosf(stackAngle);             // r * cos(u)
		    y = radius * sinf(stackAngle);              // r * sin(u)

		    // add (sectorCount+1) vertices per stack
		    // the first and last vertices have same position and normal, but different tex coords
		    for(uint32 j = 0; j <= sectorCount; ++j)
		    {
		        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

		        // vertex position (x, y, z)
		        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
		        z = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
		        newModel.AddElement3f(0, x, y, z);

				// vertex tex coord (s, t) range between [0, 1]
		        s = (float)j / sectorCount;
		        t = (float)i / stackCount;
				newModel.AddElement2f(1, s, t);

		        // normalized vertex normal (nx, ny, nz)
		        nx = x * lengthInv;
		        ny = y * lengthInv;
		        nz = z * lengthInv;

				newModel.AddElement3f(2, nx, ny, nz);
				newModel.AddElement3f(3, color.x, color.y, color.z);
		    }
		}

		// generate CCW index list of sphere triangles
		uint32 k1, k2;
		for(uint32 i = 0; i < stackCount; ++i)
		{
		    k1 = i * (sectorCount + 1);     // beginning of current stack
		    k2 = k1 + sectorCount + 1;      // beginning of next stack

		    for(uint32 j = 0; j < sectorCount; ++j, ++k1, ++k2)
		    {
		        // 2 triangles per sector excluding first and last stacks
		        // k1 => k2 => k1+1
		        if(i != 0)
		        {
					newModel.AddIndices3i(k1, k2, k1 + 1);
		        }

		        // k1+1 => k2 => k2+1
		        if(i != (stackCount-1))
		        {
					newModel.AddIndices3i(k1 + 1, k2, k2 + 1);
		        }
		    }
		}
        
        return newModel;
	}

	inline static IndexedModel CreateLine(vec3& color)
	{
		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Color
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix


		newModel.AddElement3f(0, 0, 0, 0);
        newModel.AddElement2f(1, 0, 0);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);
		newModel.AddIndices2i(0, 1);

		newModel.AddElement3f(0, 1, 1, 1);
        newModel.AddElement2f(1, 0, 0);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);
		newModel.AddIndices2i(1, 0);

		return newModel;
	}

	inline static IndexedModel CreateQuad(vec3 color = vec3(1, 0, 0))
	{
		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Color
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix

		newModel.AddElement3f(0, 1, 1, 0);
        newModel.AddElement2f(1, 1, 1);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);

		newModel.AddElement3f(0, -1, -1, 0);
        newModel.AddElement2f(1, 0, 0);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);

		newModel.AddElement3f(0, -1, 1, 0);
        newModel.AddElement2f(1, 0, 1);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);

		newModel.AddElement3f(0, -1, -1, 0);
        newModel.AddElement2f(1, 0, 0);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);

		newModel.AddElement3f(0, 1, 1, 0);
        newModel.AddElement2f(1, 1, 1);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);

		newModel.AddElement3f(0, 1, -1, 0);
        newModel.AddElement2f(1, 1, 0);
        newModel.AddElement3f(2, 0, 0, 0);
		newModel.AddElement3f(3, color.x, color.y, color.z);


		newModel.AddIndices3i(0, 1, 2);

		newModel.AddIndices3i(1, 0, 5);


		return newModel;
	}

	inline static IndexedModel CreateCube(vec3 color = vec3(1, 0, 0));

	

	inline uint32 GetId()
	{
		return id;
	}

	inline uint32 GetNumPoints()
	{
		return numVertices;
	}

private:
	uint32 id;
	bool init = 0;

	uint32 numVertices;
	uint32 VAO;
	uint32 shaderProgram;
};

IndexedModel PrimitiveGenerator::CreateCube(vec3 color /*= vec3(1, 0, 0)*/)
{



	IndexedModel newModel;


	newModel.AllocateElement(3); // Positions
	newModel.AllocateElement(2); // TexCoords
	newModel.AllocateElement(3); // Normals
	newModel.AllocateElement(3); // Color
	newModel.SetInstancedElementStartIndex(4); // Begin instanced data
	newModel.AllocateElement(16); // Transform matrix


	//Vector3f(-1, -1, -1),
	//	Vector3f(1, -1, -1),
	//	Vector3f(1, 1, -1),
	//	Vector3f(-1, 1, -1),
	//	Vector3f(-1, -1, 1),
	//	Vector3f(1, -1, 1),
	//	Vector3f(1, 1, 1),
	//	Vector3f(-1, 1, 1)
	
	newModel.AddElement3f(0, -1, -1, -1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, 1, -1, -1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, 1, 1, -1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, -1, 1, -1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);
	
	newModel.AddElement3f(0, -1, -1, 1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, 1, -1, 1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, 1, 1, 1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	newModel.AddElement3f(0, -1, 1, 1);
	newModel.AddElement3f(3, color[0], color[1], color[2]);

	//Normals broken
	newModel.AddElement3f(2, 0, 0, 1);
	newModel.AddElement3f(2, 1, 0, 0);
	newModel.AddElement3f(2, 0, 0, -1);
	newModel.AddElement3f(2, -1, 0, 0);
	newModel.AddElement3f(2, 0, 1, 0);
	newModel.AddElement3f(2, 0, -1, 0);

	newModel.AddElement2f(1, 0, 0);
	newModel.AddElement2f(1, 1, 0);
	newModel.AddElement2f(1, 1, 1);
	newModel.AddElement2f(1, 1, 1);


	newModel.AddIndices3i(0, 1, 3);
	newModel.AddIndices3i(3, 1, 2);
	newModel.AddIndices3i(1, 5, 2);
	newModel.AddIndices3i(2, 5, 6);
	newModel.AddIndices3i(5, 4, 6);
	newModel.AddIndices3i(6, 4, 7);
	newModel.AddIndices3i(4, 0, 7);
	newModel.AddIndices3i(7, 0, 3);
	newModel.AddIndices3i(3, 2, 7);
	newModel.AddIndices3i(7, 2, 6);
	newModel.AddIndices3i(4, 5, 0);
	newModel.AddIndices3i(0, 5, 1);



	return newModel;
}
