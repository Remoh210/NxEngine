#include "cBulletRigidBody.h"
#include "nConvert.h"
#include "bullet_shapes.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <mat4x4.hpp> 
#include <gtc/matrix_transform.hpp>
#include "Common/Common.h"

namespace nPhysics {

	cBulletRigidBody::cBulletRigidBody(const sRigidBodyDef& def, iShape* shape)
		:mShape(shape)
	{

		//For Collision detection
		this->bHasCollided = false;
		this->mGameObjectName = def.GameObjectName;
		switch (shape->GetShapeType())
		{
		case nPhysics::SHAPE_TYPE_PLANE:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletPlaneShape*>(shape)->GetBulletShape();
			btTransform startTransform;
			startTransform.setIdentity();
			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, 0, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.0;
			mBody = new btRigidBody(rbInfo);
			
			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			mBody->setUserPointer(this);

			break;
		}
		case nPhysics::SHAPE_TYPE_SPHERE:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletSphereShape*>(shape)->GetBulletShape();
			colShape->setLocalScaling(nConvert::ToBullet(def.Scale));
			btTransform startTransform;
			startTransform.setIdentity();
			
			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}
			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));
			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.2;
			mBody = new btRigidBody(rbInfo);
			mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
			mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
			
			mBody->setSleepingThresholds(0.0f, 0.0f);

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			
			mBody->setUserPointer(this);

			break;
		}

		case nPhysics::SHAPE_TYPE_CYLINDER:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletCylinderShape*>(shape)->GetBulletShape();
			colShape->setLocalScaling(nConvert::ToBullet(def.Scale));
			btTransform startTransform;
			startTransform.setIdentity();

			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}
			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));
			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.2;
			mBody = new btRigidBody(rbInfo);
			mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
			mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
			mBody->setSleepingThresholds(0.0f, 0.0f);

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			mBody->setUserPointer(this);

			break;
		}

		case nPhysics::SHAPE_TYPE_BOX:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletBoxShape*>(shape)->GetBulletShape();
			colShape->setLocalScaling(nConvert::ToBullet(def.Scale));
			btTransform startTransform;
			startTransform.setIdentity();
			

			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}

			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));
			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			rbInfo.m_restitution = 0.2;
			rbInfo.m_friction = 10.2;
			mBody = new btRigidBody(rbInfo);
			mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
			mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
			mBody->setSleepingThresholds(0.0f, 0.0f);

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			mBody->setUserPointer(this);

			break;
		}

		case nPhysics::SHAPE_TYPE_CAPSULE:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletCapsuleShape*>(shape)->GetBulletShape();
			btTransform startTransform;
			startTransform.setIdentity();

			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}
			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));
		

			if (!def.isPlayer)
			{
				mMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
				rbInfo.m_restitution = 0.9;
				rbInfo.m_friction = 10.2;
				mBody = new btRigidBody(rbInfo);
				mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
				mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
				mBody->setSleepingThresholds(0.0f, 0.0f);
				

			}
			else
			{
				mMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
				rbInfo.m_restitution = 0.0;
				rbInfo.m_friction = 0;
				mBody = new btRigidBody(rbInfo);
				mBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
				mBody->setSleepingThresholds(0.0f, 0.0f);
				
			}

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			mBody->setUserPointer(this);

			break;
		}
		case nPhysics::SHAPE_TYPE_MESH:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletMeshCollider*>(shape)->GetBulletShape();
			btTransform startTransform;
			colShape->setLocalScaling(nConvert::ToBullet(def.Scale));
			startTransform.setIdentity();

			mMass = def.Mass;
			btScalar mass(mMass);

			btVector3 localInertia(0, 0, 0);

			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			btQuaternion quatRot;
			quatRot.setEuler(def.Orientation.y, def.Orientation.x, def.Orientation.z);
			startTransform.setRotation(quatRot);


			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.2;
			mBody = new btRigidBody(rbInfo);
			mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
			mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
			mBody->setSleepingThresholds(0.0f, 0.0f);

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			mBody->setUserPointer(this);

			break;
		}
		case nPhysics::SHAPE_TYPE_CONVEX_HULL:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletConvexHullCollider*>(shape)->GetBulletShape();
			btTransform startTransform;
			colShape->setLocalScaling(nConvert::ToBullet(def.Scale));
			startTransform.setIdentity();

			mMass = def.Mass;
			btScalar mass(mMass);

			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0.0f, 0, 0);
			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}

			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));

			
			if (!def.isPlayer)
			{
				mMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
				rbInfo.m_restitution = 0.3;
				rbInfo.m_friction = 10.2;
				mBody = new btRigidBody(rbInfo);
				mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
				mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
				mBody->setSleepingThresholds(0.0f, 0.0f);
				mBody = new btRigidBody(rbInfo);


			}
			else
			{
				mMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
				rbInfo.m_restitution = 0.0;
				rbInfo.m_friction = 0;
				mBody = new btRigidBody(rbInfo);
				mBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
				mBody->setSleepingThresholds(0.0f, 0.0f);
				mBody = new btRigidBody(rbInfo);

			}


			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.2;
			

			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			if (def.isGhostShape)
			{
				mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			mBody->setUserPointer(this);

			break;
		}
		default:
		{
			break;
		}
		}
	}

	cBulletRigidBody::~cBulletRigidBody()
	{
		mBody->setUserPointer(0);
		delete mBody;
		mBody = 0;
		delete mMotionState;
		mMotionState = 0;
	}

	iShape * cBulletRigidBody::GetShape()
	{
		return mShape;
	}

	glm::mat4 cBulletRigidBody::GetTransform()
	{	
		btTransform tranform;
		mMotionState->getWorldTransform(tranform);	
		return nConvert::ToSimple(tranform);
	}

	glm::vec3 cBulletRigidBody::GetPosition()
	{

		btVector3 btVec = this->mBody->getCenterOfMassPosition();
		return nConvert::ToSimple(btVec);
	}

	glm::vec3 cBulletRigidBody::GetEulerRotation()
	{
		btTransform tranform;
		tranform =	mBody->getWorldTransform();
		return nConvert::BtQuatToEuler(tranform.getRotation());
	}

	glm::mat4 cBulletRigidBody::GetMatRotation()
	{
		btQuaternion btQuat = this->mBody->getOrientation();

		glm::quat quatRot = nConvert::ToSimple(btQuat);
		return glm::toMat4(quatRot);
	}

	float cBulletRigidBody::GetMass()
	{
		return this->mMass;
	}

	bool cBulletRigidBody::GetCollision()
	{
		return this->bHasCollided;
	}

	glm::vec3 cBulletRigidBody::GetVelocity()
	{
		btVector3 vel = this->mBody->getLinearVelocity();
		return nConvert::ToSimple(vel);
	}

	glm::vec3 cBulletRigidBody::GetAccel()
	{
		return glm::vec3();
	}

	glm::vec3 cBulletRigidBody::GetAngulatVelocity()
	{
		return nConvert::ToSimple(mBody->getAngularVelocity());
	}

	std::string cBulletRigidBody::GetGOName()
	{
		return mGameObjectName;
	}

	void cBulletRigidBody::SetPosition(glm::vec3 position)
	{
		btTransform transform = mBody->getCenterOfMassTransform();
		transform.setOrigin(nConvert::ToBullet(position));
		mBody->setCenterOfMassTransform(transform);
	}

	void cBulletRigidBody::ApplyForce(glm::vec3 force, glm::vec3 relPos)
	{
		this->mBody->applyForce(nConvert::ToBullet(force), nConvert::ToBullet(relPos));
	}

	void cBulletRigidBody::ApplyImpulse(glm::vec3 force, glm::vec3 relPos)
	{
		mBody->applyImpulse(nConvert::ToBullet(force), nConvert::ToBullet(relPos));
	}

	void cBulletRigidBody::SetGravity(glm::vec3 gravity)
	{
		mBody->setGravity(nConvert::ToBullet(gravity));
	}


	void cBulletRigidBody::SetEulerRotation(glm::vec3 rotation)
	{
	}

	void cBulletRigidBody::SetMatRotation(glm::mat4 rotation)
	{
		btTransform transform = mBody->getCenterOfMassTransform();
		transform.setRotation(nConvert::ToBullet(rotation));
		mBody->setCenterOfMassTransform(transform);
	}

	void cBulletRigidBody::SetMass(float mass)
	{
	}

	void cBulletRigidBody::SetVelocity(glm::vec3 velocity)
	{
		this->mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}
	void cBulletRigidBody::SetAccel(glm::vec3 accel)
	{
	}
	void cBulletRigidBody::SetCollision(bool coll)
	{
		this->bHasCollided = coll;
	}
}