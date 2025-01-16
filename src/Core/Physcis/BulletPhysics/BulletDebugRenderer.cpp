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
	}

	void BulletDebugRenderer::clearLines()
	{
		meshToDraw = DebugMesh();
		meshToDraw.model.AllocateElement(3); // Positions
		meshToDraw.model.AllocateElement(3); // Colors
		meshToDraw.model.SetInstancedElementStartIndex(2); // Begin instanced data
		meshToDraw.model.AllocateElement(16); // Transform matrix
	}

	void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		drawLine(from, to, color, color);
	}

	void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
		meshToDraw.model.AddElement3f(0, from[0], from[1], from[2]);
		meshToDraw.model.AddElement3f(0, to[0], to[1], to[2]);
		meshToDraw.model.AddElement3f(1, fromColor[0], fromColor[1], fromColor[2]);
		meshToDraw.model.AddElement3f(1, toColor[0], toColor[1], toColor[2]);
	}

	void BulletDebugRenderer::flushLines()
	{
		if (false == meshToDraw.model.GetElement(0).empty())
		{
			DebugRenderer::DrawModelAsArrays(meshToDraw.model);
		}
	}
}