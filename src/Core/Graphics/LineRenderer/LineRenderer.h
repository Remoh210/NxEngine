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


	IndexedModel CreateGridVA(uint32 slices)
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
        
        //Array<vec3> vertices;
        for(int j=0; j<=slices; ++j)
        {
          for(int i=0; i<=slices; ++i)
          {
            float x = (float)i/(float)slices;
            float y = 0;
            float z = (float)j/(float)slices;
            newModel.AddElement3f(0, x, y, z);
            newModel.AddElement2f(1, 0, 0);
            newModel.AddElement3f(2, 1, 0, 0);
            newModel.AddElement3f(3, 0, 0, 0);
          }
        }

//		for (uint32 i = 0; i < inVertices.size(); i++)
//        {
//			vec3 pos = inVertices[i];
//
//			newModel.AddElement3f(0, pos.x, pos.y, pos.z);
//			newModel.AddElement2f(1, 0, 0);
//			newModel.AddElement3f(2, 1, 0, 0);
//			newModel.AddElement3f(3, 0, 0, 0);
//		}
        
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

	//inline void AddPoint(vec3& pointIn)
	//{
	//	Points.push_back(pointIn);
	//}
    
    IndexedModel inline CreateGrid(uint slices)
    {
//        Array<vec3> vertices;
//        for(int j=0; j<=slices; ++j)
//        {
//          for(int i=0; i<=slices; ++i)
//          {
//            float x = (float)i/(float)slices;
//            float y = 0;
//            float z = (float)j/(float)slices;
//            vertices.push_back(vec3(x, y, z));
//          }
//        }
//        return CreateVertexArray(vertices);
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
