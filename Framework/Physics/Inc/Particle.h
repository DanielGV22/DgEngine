#pragma once

#include "RigidBody.h"
#include "CollisionShape.h"

namespace DgEngine::Physics
{
	struct ParticleInfo
	{
		float lifeTime = 0.0f;
		Math::Vector3 position = Math::Vector3::Zero;
		Math::Vector3 velocity = Math::Vector3::Zero;
		Graphics::Color startColor = Graphics::Colors::White;
		Graphics::Color endColor = Graphics::Colors::White;
		Math::Vector3 startScale = Math::Vector3::One;
		Math::Vector3 endScale = Math::Vector3::One;
	};

	class Particle
	{
	public:
		void Initialize();
		void Terminate();
		void Activate(const ParticleInfo& info);
		void Update(float deltaTime);

		bool IsActive() const;
		const Graphics::Transform& GetTransform() const;
		Graphics::Color GetColor()const;
		const Math::Vector3& GetPosition() const;

	private:
		Graphics::Transform mTransform;
		RigidBody mRigidBody;
		CollisionShape mCollisionShape;

		ParticleInfo mInfo;
		float mLifeTime = 0.0f;

	};
}