#pragma once
#include <Core/Physcis/iShape.h>
#include <Core/Physcis/shapes.h>
#include <Core/Physcis/GL_Vertex.h>
#include <Core/Physcis/GL_Triangle.h>
#include "btBulletCollisionCommon.h"

namespace nPhysics
{
	
	class cBulletSphereShape : public iSphereShape
	{
	public:
		cBulletSphereShape(float radius);
		virtual ~cBulletSphereShape();
		virtual bool GetSphereRadius(float& radiusOut) { radiusOut = this->mRadius; return true; }
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}
	protected:
		float mRadius;
		cBulletSphereShape(const cBulletSphereShape& other) : iSphereShape(other) {}
		cBulletSphereShape& operator=(const cBulletSphereShape& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletPlaneShape :  public iPlaneShape
	{
	public:
		cBulletPlaneShape(const glm::vec3& normal, float planeConst);
		virtual ~cBulletPlaneShape();
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut);
		virtual bool GetPlaneConstant(float& planeConstantOut);
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		glm::vec3 mNormal;
		float mCnonst;
		cBulletPlaneShape(const cBulletPlaneShape& other) : iPlaneShape(other) {}
		cBulletPlaneShape& operator=(const cBulletPlaneShape& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletCylinderShape : public iCylinderShape
	{
	public:
		cBulletCylinderShape(const glm::vec3& halfExtents, int axis);
		virtual ~cBulletCylinderShape();
		virtual float GetCylinderRadius();
		virtual float GetCylinderHeight();
		virtual int GetCylinderAxis();
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		float mRadius;
		int mAxis;
		cBulletCylinderShape(const cBulletCylinderShape& other) : iCylinderShape(other) {}
		cBulletCylinderShape& operator=(const cBulletCylinderShape& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletCapsuleShape : public iCapsuleShape
	{
	public:
		cBulletCapsuleShape(float height, float radius, int axis);
		virtual ~cBulletCapsuleShape();
		virtual float GetCapsuleRadius();
		virtual float GetCapsuleHeight();
		virtual int GetCapsuleAxis();
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		float mRadius;
		int mAxis;
		float mHeight;
		cBulletCapsuleShape(const cBulletCapsuleShape& other) : iCapsuleShape(other) {}
		cBulletCapsuleShape& operator=(const cBulletCapsuleShape& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletBoxShape : public iBoxShape
	{
	public:
		cBulletBoxShape(const glm::vec3& halfExtents);
		virtual ~cBulletBoxShape();
		inline glm::vec3 GetHalfExtents()
		{
			return mHalfExtents;
		};
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		glm::vec3 mHalfExtents;
		cBulletBoxShape(const cBulletBoxShape& other) : iBoxShape(other) {}
		cBulletBoxShape& operator=(const cBulletBoxShape& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletMeshCollider : public iMeshCollider
	{
	public:
		cBulletMeshCollider(const GL_Triangle* triangles, size_t numOfTriangles);
		virtual ~cBulletMeshCollider();
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		glm::vec3 mHalfExtents;
		cBulletMeshCollider(const cBulletMeshCollider& other) : iMeshCollider(other) {}
		cBulletMeshCollider& operator=(const cBulletMeshCollider& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

	class cBulletConvexHullCollider : public iConvexHullCollider
	{
	public:
		cBulletConvexHullCollider(const float* vertecies, size_t numOfVertecies);
		virtual ~cBulletConvexHullCollider();
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		glm::vec3 mHalfExtents;
		cBulletConvexHullCollider(const cBulletConvexHullCollider& other) : iConvexHullCollider(other) {}
		cBulletConvexHullCollider& operator=(const cBulletConvexHullCollider& other) { return *this; }
		btCollisionShape* mBulletShape;
	};

}