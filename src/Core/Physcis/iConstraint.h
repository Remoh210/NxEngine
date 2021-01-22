#pragma once
#include "iRigidBody.h"
#include "eConstraintType.h"

namespace nPhysics
{
	class iConstraint
	{
	public:
		virtual ~iConstraint() {}

		//virtual iRigidBody* GetRigidBodyA() = 0;
		//virtual iRigidBody* GetRigidBodyB() = 0;

		inline eConstraintType GetConstraintType() const { return mConstraintType; }

	protected:
		// The constructor is protector, only accessible by the subclasses
		iConstraint(eConstraintType constraintType) : mConstraintType(constraintType) {}
		iConstraint() {}
		iConstraint(const iConstraint& other) {}
		iConstraint& operator=(const iConstraint& other) { return *this; }

	private:
		eConstraintType mConstraintType;
	};
}