#include "Precompiled.h"
#include "Particle.h"

#include "PhysicsWorld.h"

using namespace DgEngine;
using namespace DgEngine::Physics;


void  Particle::Initialize()
{
	mLifeTime = 0.0f;
	mCollisionShape.InitializeEmpty();
	mRigidBody.Initialize(mTransform, mCollisionShape, 1.0f, false);
	mRigidBody.SetCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

}
void Particle::Terminate()
{
	mRigidBody.Terminate();
	mCollisionShape.Terminate();
}
void Particle::Activate(const ParticleInfo& info)
{
	mInfo = info;
	mLifeTime = info.lifeTime;
	if (info.lifeTime > 0)
	{
		mRigidBody.Activate();
	}
	mRigidBody.SetPosition(info.position);
	mRigidBody.SetVelocity(info.velocity);
}
void Particle::Update(float deltaTime)
{
	if (IsActive())
	{
		mLifeTime -= deltaTime;
		if (!IsActive())
		{
			mRigidBody.Deactivate();
		}
		else
		{
			const float t = 1.0f - Math::Clamp(mLifeTime / mInfo.lifeTime, 0.0f, 1.0f);
			mTransform.scale = Math::Lerp(mInfo.startScale, mInfo.endScale, t);
		}
	}
}

bool Particle::IsActive() const
{
	return mLifeTime > 0.0f;
}
const Graphics::Transform& Particle::GetTransform() const
{
	return mTransform;
}
Graphics::Color Particle::GetColor()const
{
	const float t = 1.0f - Math::Clamp(mLifeTime / mInfo.lifeTime, 0.0f, 1.0f);
	return Math::Lerp(mInfo.startColor, mInfo.endColor, t);
}
const Math::Vector3& Particle::GetPosition() const
{
	return mTransform.position;
}
