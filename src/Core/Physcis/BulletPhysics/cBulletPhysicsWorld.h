#pragma once
#include <Core/Physcis/iPhysicsWorld.h>
#include <Core/Physcis/iConstraint.h>
//#include <Core/Physcis/iRigidBody.h>
#include "btBulletDynamicsCommon.h"
#include <stdio.h>
#include <string>

namespace nPhysics {
	class cBulletPhysicsWorld : public iPhysicsWorld
	{
	public:
		cBulletPhysicsWorld();
		~cBulletPhysicsWorld();

		void SetGravity(const glm::vec3& gravity);
		bool AddBody(iRigidBody* body);
		bool RemoveBody(iRigidBody* body);

		//Constraints
		virtual void AddConstraint(iConstraint* constraint);
		virtual void RemoveConstraint(iConstraint* constraint);
		virtual std::pair<std::string, std::string> GetLastColPair();
		virtual bool RayCast(glm::vec3& from, glm::vec3& to);
		virtual iRigidBody* RayCastGetObject(glm::vec3& from, glm::vec3& to);
		void Update(float dt);

	protected:
		cBulletPhysicsWorld(const cBulletPhysicsWorld& other) {}
		cBulletPhysicsWorld& operator=(const cBulletPhysicsWorld& other) { return *this; }
	private:

		btDefaultCollisionConfiguration* mCollisionConfiguration;

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher 
		btCollisionDispatcher* mDispatcher;

		//btDbvtBroadphase is a good general purpose broadphase. 
		btBroadphaseInterface* mOverlappingPairCache;

		//the default constraint solver. For parallel processing you can use a different solver 
		btSequentialImpulseConstraintSolver* mSolver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* mDynamicsWorld;

	};

}