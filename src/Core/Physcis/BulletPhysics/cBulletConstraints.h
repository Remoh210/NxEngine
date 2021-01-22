#pragma once

#include <vector>
#include <Core/Physcis/iConstraint.h>
#include <Core/Physcis/iRigidBody.h>
#include "cBulletRigidBody.h"

namespace nPhysics
{
	class cBulletPoinToPointConstraint : public iConstraint
	{
	public:
		cBulletPoinToPointConstraint(cBulletRigidBody* rb, const btVector3& pivot);
		cBulletPoinToPointConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& pivotInA, const btVector3& pivotInB);
		virtual ~cBulletPoinToPointConstraint();


		virtual btTypedConstraint* GetTypedConstraint() { return mConstraint; }

	private:
		btPoint2PointConstraint* mConstraint;
		cBulletPoinToPointConstraint(const cBulletPoinToPointConstraint& other) {}
		cBulletPoinToPointConstraint& operator=(const cBulletPoinToPointConstraint& other) { return *this; }

	private:
		eConstraintType mConstraintType;
	};

	class cBulletHingeConstraint : public iConstraint
	{
	public:
		cBulletHingeConstraint(cBulletRigidBody* rb, const btVector3& pivot, const btVector3& axis);
		cBulletHingeConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& pivotInA, const btVector3& pivotInB, const btVector3& axisInA, const btVector3& axisInB);
		virtual ~cBulletHingeConstraint();


		virtual btTypedConstraint* GetTypedConstraint() { return mConstraint; }

	private:
		btHingeConstraint* mConstraint;
		cBulletHingeConstraint(const cBulletHingeConstraint& other) {}
		cBulletHingeConstraint& operator=(const iConstraint& other) { return *this; }

	private:
		eConstraintType mConstraintType;
	};

}