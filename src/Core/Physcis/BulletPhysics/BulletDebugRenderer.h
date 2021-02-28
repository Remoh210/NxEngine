#pragma once

#include "Core/Physcis/iDebugRenderer.h"
#include "rendering/IndexedModel.h"
#include "rendering/Shader.h"
#include "LinearMath/btIDebugDraw.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
namespace nPhysics
{

	class BulletDebugRenderer : public btIDebugDraw, public iDebugRenderer
	{

	public:
		BulletDebugRenderer(btDynamicsWorld* worldIn);

		virtual void clearLines();

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

		virtual void flushLines();

		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, 
			btScalar distance, int lifeTime, const btVector3& color) {}
		virtual void reportErrorWarning(const char* warningString) {}
		virtual void draw3dText(const btVector3& location, const char* textString) {}
		virtual void setDebugMode(int debugMode) { mDebugMode = debugMode; }
		virtual int getDebugMode() const { return mDebugMode; }

	private:

		struct DebugMesh
		{
			IndexedModel model;
			vec3 color;

		};
		DebugMesh meshToDraw;

		int mDebugMode;

	};
}