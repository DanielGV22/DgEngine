#include "Precompiled.h"
#include "RigidBody.h"
#include "CollisionShape.h"
#include "PhysicsWorld.h"

using namespace DgEngine;
using namespace DgEngine::Physics;

RigidBody::~RigidBody()
{
	ASSERT(mRigidBody == nullptr, "RigidBody: terminate must be called!");
}

void RigidBody::Initialize(Graphics::Transform& graphicsTransform, const CollisionShape& shape, float mass, bool addToWorld)
{
	mGraphicsTransform = &graphicsTransform;
	mMass = mass;

	btVector3 localInertia = btVector3();
	//shape.mCollisionShape->calculateLocalInertia(mMass, localInertia);
	mMotionState = new btDefaultMotionState(ConvertTobtTransform(graphicsTransform));
	mRigidBody = new btRigidBody(mMass, mMotionState, shape.mCollisionShape, localInertia);

	if (addToWorld)
	{

		PhysicsWorld::Get()->Register(this);

	}
}
void RigidBody::Terminate()
{
	PhysicsWorld::Get()->Unregister(this);
	SafeDelete(mRigidBody);
	SafeDelete(mMotionState);
}

void RigidBody::Activate()
{
	PhysicsWorld::Get()->Register(this);
	mRigidBody->activate();
}

void RigidBody::Deactivate()
{
	PhysicsWorld::Get()->Unregister(this);
}

void RigidBody::SetCollisionFlags(int flags)
{
	mRigidBody->setCollisionFlags(flags);
}

void RigidBody::SetPosition(const Math::Vector3& position)
{
	mRigidBody->activate();
	mGraphicsTransform->position = position;
	mRigidBody->setWorldTransform(ConvertTobtTransform(*mGraphicsTransform));
}

void RigidBody::SetVelocity(const Math::Vector3& velocity)
{
	mRigidBody->activate();
	mRigidBody->setLinearVelocity(TobtVector3(velocity));
}

const Math::Vector3 RigidBody::GetVelocity() const
{
	return ToVector3(mRigidBody->getLinearVelocity());
}

bool RigidBody::IsDynamic() const
{
	return mMass > 0.0f;
}

void RigidBody::SyncWithGraphics()
{
	const btTransform& worldTransform = mRigidBody->getWorldTransform();
	mGraphicsTransform->position = ToVector3(worldTransform.getOrigin());
	mGraphicsTransform->rotation = ToQuaternion(worldTransform.getRotation());
}

btRigidBody* RigidBody::GetRigidBody()
{
	return mRigidBody;
}
