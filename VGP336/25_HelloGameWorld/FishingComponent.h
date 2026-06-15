#pragma once

#include "CustomTypeIds.h"
#include <DgEngine/Inc/DgEngine.h>

class FishComponent;

class FishingComponent : public DgEngine::Component
{
public:
	SET_TYPE_ID(CustomComponentId::Fishing);

	void Initialize() override;
	void Update(float deltaTime) override;
	void DebugUI() override;
	void Deserialize(const rapidjson::Value& value) override;

private:
	enum class FishingState
	{
		Idle,
		ReelWindow
	};

	void DrawFishingLine() const;
	void CastLine();

	DgEngine::TransformComponent* mTransform = nullptr;

	float mCastRange = 8.0f;
	float mCatchRadius = 2.0f;
	float mCooldown = 1.0f;
	float mCooldownTimer = 0.0f;

	float mCastLineTimer = 0.0f;
	DgEngine::Math::Vector3 mLastCastStart = DgEngine::Math::Vector3::Zero;
	DgEngine::Math::Vector3 mLastCastEnd = DgEngine::Math::Vector3::Zero;

	int mFishCaught = 0;
	int mScore = 0;

	bool mLastCastCaughtFish = false;

	FishingState mState = FishingState::Idle;
	float mStateTimer = 0.0f;
	FishComponent* mHookedFish = nullptr;

	float mMinWaterZ = 2.0f;

	float mMinCastRange = 2.0f;
	float mMaxCastRange = 12.0f;
};