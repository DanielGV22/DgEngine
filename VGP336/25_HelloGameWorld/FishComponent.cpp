#include <random>

#include "FishComponent.h"
#include "FishService.h"

using namespace DgEngine;
using namespace DgEngine::Math;

void FishComponent::Initialize()
{
	mTransform = GetOwner().GetComponent<TransformComponent>();

	if (mTransform != nullptr)
	{
		mStartPosition = mTransform->position;
		mTargetPosition = mStartPosition;
		PickNewTarget();
	}

	FishService* fishService = GetOwner().GetWorld().GetService<FishService>();
	if (fishService != nullptr)
	{
		fishService->Register(this);
	}
}

void FishComponent::Terminate()
{
	FishService* fishService = GetOwner().GetWorld().GetService<FishService>();
	if (fishService != nullptr)
	{
		fishService->Unregister(this);
	}

	mTransform = nullptr;
}

void FishComponent::Update(float deltaTime)
{
	if (mCaught || mTransform == nullptr)
	{
		return;
	}

	Vector3 toTarget = mTargetPosition - mTransform->position;
	toTarget.y = 0.0f;

	if (MagnitudeSqr(toTarget) < 0.05f)
	{
		PickNewTarget();
		return;
	}

	Vector3 direction = Normalize(toTarget);
	mTransform->position += direction * mSwimSpeed * deltaTime;
}

void FishComponent::DebugUI()
{
	ImGui::DragFloat("Swim Speed", &mSwimSpeed, 0.1f, 0.1f, 20.0f);
	ImGui::DragFloat("Wander Radius", &mWanderRadius, 0.1f, 0.1f, 50.0f);
	ImGui::DragFloat("Catch Value", &mCatchValue, 1.0f, 0.0f, 1000.0f);
	ImGui::Checkbox("Caught", &mCaught);
}

void FishComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadFloat("SwimSpeed", mSwimSpeed, value);
	SaveUtil::ReadFloat("WanderRadius", mWanderRadius, value);
	SaveUtil::ReadFloat("CatchValue", mCatchValue, value);
}

void FishComponent::Catch()
{
	mCaught = true;

	if (mTransform != nullptr)
	{
		mTransform->position.y = -100.0f;
	}
}

Vector3 FishComponent::GetPosition() const
{
	return mTransform != nullptr ? mTransform->position : Vector3::Zero;
}

void FishComponent::PickNewTarget()
{
	static std::random_device rd;
	static std::mt19937 rng(rd());

	std::uniform_real_distribution<float> dist(-mWanderRadius, mWanderRadius);

	const float randomX = dist(rng);
	const float randomZ = dist(rng);

	mTargetPosition =
	{
		mStartPosition.x + randomX,
		mStartPosition.y,
		mStartPosition.z + randomZ
	};
}