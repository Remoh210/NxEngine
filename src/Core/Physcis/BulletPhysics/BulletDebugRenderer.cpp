#include "Common/CommonTypes.h"
#include "BulletDebugRenderer.h"
#include "Core/Graphics/DebugRenderer/DebugRenderer.h"
#include "nConvert.h"

namespace nPhysics 
{

	BulletDebugRenderer::BulletDebugRenderer(btDynamicsWorld* worldIn)
	{
		worldIn->setDebugDrawer(this);
		mDebugMode = 0;



		const std::string debugShaderText =
			"#if defined(VS_BUILD)\n"
			"\n"
			"out vec3 VertColor;\n"
			"\n"
			"layout (location = 0) in vec3 position;\n"
			"layout (location = 3) in vec3 color;\n"
			"layout (location = 4) in mat4 transformMat;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    gl_Position = transformMat * vec4(position, 1.0) ;\n"
			"    texCoord0 = texCoord;\n"
			"\tVertColor = color;\n"
			"}\n"
			"\n"
			"#elif defined(FS_BUILD)\n"
			"in vec3 VertColor;\n"
			"\n"
			"layout (location = 0) out vec4 FinalColor;\n"
			"void main()\n"
			"{\n"
			"    FinalColor = vec4(VertColor.rgb, 1);\n"
			"\n"
			"}\n"
			"#endif";
;
	}


	void BulletDebugRenderer::clearLines()
	{
		meshToDraw = DebugMesh();

		meshToDraw.model.AllocateElement(3); // Positions
		meshToDraw.model.AllocateElement(3); // Colors
		meshToDraw.model.SetInstancedElementStartIndex(2); // Begin instanced data
		meshToDraw.model.AllocateElement(16); // Transform matrix

		verticesToDraw.clear();
		bFirstLine = false;
	}

	void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{

		meshToDraw.model.AddElement3f(0, from[0], from[1], from[2]);
		meshToDraw.model.AddElement3f(0, to[0], to[1], to[2]);
		meshToDraw.model.AddElement3f(1, color[0], color[1], color[2]);
		meshToDraw.model.AddElement3f(1, color[0], color[1], color[2]);

		//drawLine(from, to, color, color);
	}

	void BulletDebugRenderer::flushLines()
	{
		if (meshToDraw.model.GetElement(0).size() > 0)
		{
			DebugRenderer::DrawModelAsArrays(meshToDraw.model);
		}
		
	}

	//void BulletDebugRenderer::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
	//{
	//	vec3 pos = nConvert::ToSimple(transform.getOrigin());
	//	DebugRenderer::DrawDebugSphereAsMesh(pos, radius, vec3(1.0f));
	//}

	

}