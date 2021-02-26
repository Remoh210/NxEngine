#pragma once
#include "iShape.h"
#include <glm.hpp>

namespace nPhysics
{
	class iSphereShape : public iShape
	{
	public:
		virtual ~iSphereShape() {}
		virtual bool GetSphereRadius(float& radiusOut) = 0;

	protected:
		iSphereShape() : iShape(SHAPE_TYPE_SPHERE) {}
		iSphereShape(const iSphereShape& other) : iShape(other) {}
		iSphereShape& operator=(const iSphereShape& other) { return *this; }
	};

	class iPlaneShape : public iShape
	{
	public:
		virtual ~iPlaneShape() {}
		virtual bool GetPlaneConstant(float& planeConstantOut) = 0;
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut) = 0;

	protected:
		iPlaneShape() : iShape(SHAPE_TYPE_PLANE) {}
		iPlaneShape(const iPlaneShape& other) : iShape(other) {}
		iPlaneShape& operator=(const iPlaneShape& other) { return *this; }
	};

	class iCylinderShape : public iShape
	{
	public:
		virtual ~iCylinderShape() {}
		virtual float GetCylinderRadius() = 0;
		virtual float GetCylinderHeight() = 0;
		virtual int GetCylinderAxis() = 0;

	protected:
		iCylinderShape() : iShape(SHAPE_TYPE_CYLINDER) {}
		iCylinderShape(const iCylinderShape& other) : iShape(other) {}
		iCylinderShape& operator=(const iCylinderShape& other) { return *this; }
	};

	class iCapsuleShape : public iShape
	{
	public:
		virtual ~iCapsuleShape() {}
		virtual float GetCapsuleRadius() = 0;
		virtual float GetCapsuleHeight() = 0;
		virtual int GetCapsuleAxis() = 0;

	protected:
		iCapsuleShape() : iShape(SHAPE_TYPE_CAPSULE) {}
		iCapsuleShape(const iCapsuleShape& other) : iShape(other) {}
		iCapsuleShape& operator=(const iCapsuleShape& other) { return *this; }
	};


	class iBoxShape : public iShape
	{
	public:
		virtual ~iBoxShape() {}
		virtual glm::vec3 GetHalfExtents() = 0;


	protected:
		iBoxShape() : iShape(SHAPE_TYPE_BOX) {}
		iBoxShape(const iBoxShape& other) : iShape(other) {}
		iBoxShape& operator=(const iBoxShape& other) { return *this; }
	};



	class iMeshCollider : public iShape
	{
	public:
		virtual ~iMeshCollider() {}


	protected:
		iMeshCollider() : iShape(SHAPE_TYPE_MESH) {}
		iMeshCollider(const iMeshCollider& other) : iShape(other) {}
		iMeshCollider& operator=(const iMeshCollider& other) { return *this; }
	};

	class iConvexHullCollider : public iShape
	{
	public:
		virtual ~iConvexHullCollider() {}


	protected:
		iConvexHullCollider() : iShape(SHAPE_TYPE_CONVEX_HULL) {}
		iConvexHullCollider(const iConvexHullCollider& other) : iShape(other) {}
		iConvexHullCollider& operator=(const iConvexHullCollider& other) { return *this; }
	};
}