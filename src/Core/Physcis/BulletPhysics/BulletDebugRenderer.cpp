#include "Common/CommonTypes.h"
#include "BulletDebugRenderer.h"
#include "Core/Graphics/DebugRenderer/DebugRenderer.h"
#include "nConvert.h"

namespace nPhysics 
{

	void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		vec3 fromLoc = nConvert::ToSimple(from);
		vec3 toLoc = nConvert::ToSimple(to);
		vec3 col = nConvert::ToSimple(color);

		DebugRenderer::DrawDebugLine(fromLoc, toLoc, 0.0f, col);
	}
}