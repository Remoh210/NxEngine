#pragma once
#include <Common/Common.h>
#include <Common/CommonTypes.h>
#include <Common/dataStructs/Array.h>
#include <rendering/RenderDevice.h>
#include <iostream>

class LineRenderer
{
public:
	LineRenderer() {}

	~LineRenderer() {}


	static IndexedModel CreateGridVA(uint32 slices, vec3 color = vec3(1.0f, 0.0f, 0.0f))
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
        for(int j=0; j<=slices; ++j)
        {
          for(int i=0; i<=slices; ++i)
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

        for(int j=0; j<slices; ++j)
        {
          for(int i=0; i<slices; ++i)
          {

            int row1 =  j    * (slices+1);
            int row2 = (j+1) * (slices+1);
              
            newModel.AddIndices4i(row1+i, row1+i+1, row1+i+1,row2+i+1);
            newModel.AddIndices4i(row2+i+1, row2+i, row2+i, row1+i);
            

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
