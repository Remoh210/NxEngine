#pragma once
#include <Common/Common.h>
#include <Common/CommonTypes.h>
#include <Common/dataStructs/Array.h>
#include <rendering/RenderDevice.h>
#include <iostream>

#define PI 3.14

class LineRenderer
{
public:
	LineRenderer() {}

	~LineRenderer() {}


	inline static IndexedModel CreateGridVA(uint32 slices, vec3 color = vec3(1.0f, 0.0f, 0.0f))
	{
//		Array<float> vertices;
//		numVertices = inVertices.size();
//		for (size_t i = 0; i < numVertices; i++)
//		{
//			vertices.push_back(inVertices[i].x);
//			vertices.push_back(inVertices[i].y);
//			vertices.push_back(inVertices[i].z);
//		}
//
//		for (int i = 0; i < 9; i++)
//		{
//			std::cout << vertices[i] << "," << std::endl;
//		}

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

	inline static IndexedModel CreateSphere(float radius, uint32 sectorCount, uint32 stackCount, vec3& color)
	{
		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Color
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix

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
		    z = radius * sinf(stackAngle);              // r * sin(u)

		    // add (sectorCount+1) vertices per stack
		    // the first and last vertices have same position and normal, but different tex coords
		    for(uint32 j = 0; j <= sectorCount; ++j)
		    {
		        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

		        // vertex position (x, y, z)
		        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
		        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
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


	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

};
