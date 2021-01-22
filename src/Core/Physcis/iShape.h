#pragma once
#include "eShapeType.h"
#include <glm.hpp>
namespace nPhysics
{
	class iShape
	{
	public: 
		virtual ~iShape() {};

		inline eShapeType GetShapeType() const { return mShapeType; }
		//virtual bool GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) { return false; } /*=0*/
		virtual bool GetSphereRadius(float& radiusOut) { return false; }
		virtual bool GetPlaneConstant(float& planeConstantOut) { return false; }
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut) { return false; }
		//Cylinder
		virtual float GetCylinderRadius() { return 0; }
		virtual int GetCylinderAxis() { return 0; }
		virtual float GetCylinderHeight() { return 0.0f; }
		//Capsule
		virtual float GetCapsuleRadius() { return 0.0f; }
		virtual float GetCapsuleHeight() { return 0.0f; }
		virtual int	GetCapsuleAxis() { return 0; }

		//Box
		virtual glm::vec3 GetHalfExtents() { return glm::vec3(0.f); }

	protected: 
		iShape(eShapeType shapeType) : mShapeType(shapeType){}
		iShape(const iShape& other){}
		iShape& operator=(const iShape& other) { return *this; }
	private:
		eShapeType mShapeType;
	};

}