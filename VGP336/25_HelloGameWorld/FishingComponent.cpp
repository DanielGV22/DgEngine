#include "FishingComponent.h"
#include "FishService.h"
#include "FishComponent.h"

#include <Windows.h>
#include <algorithm>

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Math;

void FishingComponent::Initialize()
{
	mTransform = GetOwner().GetComponent<TransformComponent>();
	ASSERT(mTransform != nullptr, "FishingComponent: requires TransformComponent.");
}

void FishingComponent::Update(float deltaTime)
{
	if (mCooldownTimer > 0.0f)
	{
		mCooldownTimer -= deltaTime;
	}

	if (mState == FishingState::ReelWindow)
	{
		mStateTimer -= deltaTime;

		if (InputSystem::Get()->IsKeyPressed(KeyCode::F))
		{
			if (mHookedFish != nullptr)
			{
				mHookedFish->Catch();
				mFishCaught++;
				mScore += 10;
			}

			mHookedFish = nullptr;
			mState = FishingState::Idle;
			mLastCastCaughtFish = false;
		}
		else if (mStateTimer <= 0.0f)
		{
			mHookedFish = nullptr;
			mState = FishingState::Idle;
			mLastCastCaughtFish = false;
		}
	}

	if (mCastLineTimer > 0.0f)
	{
		mCastLineTimer -= deltaTime;
	}

	if (mCastLineTimer > 0.0f || mState == FishingState::ReelWindow)
	{
		DrawFishingLine();
	}

	if (mState == FishingState::Idle &&
		InputSystem::Get()->IsKeyPressed(KeyCode::F))
	{
		CastLine();
	}
}

void FishingComponent::DebugUI()
{
	ImGui::DragFloat("Cast Range", &mCastRange, 0.1f, 1.0f, 50.0f);
	ImGui::DragFloat("Catch Radius", &mCatchRadius, 0.1f, 0.1f, 20.0f);
	ImGui::DragFloat("Cooldown", &mCooldown, 0.1f, 0.0f, 10.0f);
	ImGui::Text("Cooldown Timer: %.2f", mCooldownTimer);
	ImGui::Text("Last Cast: %s", mLastCastCaughtFish ? "Caught Fish" : "Missed");
	ImGui::Text("Fish Caught: %d", mFishCaught);
	ImGui::Text("Score: %d", mScore);
	if (mState == FishingState::ReelWindow)
	{
		ImGui::Text("FISH ON THE LINE!");
		ImGui::Text("Press F again to reel in!");
		ImGui::Text("Reel Timer: %.2f", mStateTimer);
	}
}

void FishingComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadFloat("CastRange", mCastRange, value);
	SaveUtil::ReadFloat("CatchRadius", mCatchRadius, value);
	SaveUtil::ReadFloat("Cooldown", mCooldown, value);
	SaveUtil::ReadFloat("MinWaterZ", mMinWaterZ, value);
	SaveUtil::ReadFloat("MinCastRange", mMinCastRange, value);
	SaveUtil::ReadFloat("MaxCastRange", mMaxCastRange, value);
}

void FishingComponent::DrawFishingLine() const
{
	const int segments = 12;
	const float sagAmount = 0.75f;

	Color lineColor = mLastCastCaughtFish ? Colors::Green : Colors::Yellow;

	Vector3 previous = mLastCastStart;

	for (int i = 1; i <= segments; ++i)
	{
		const float t = static_cast<float>(i) / static_cast<float>(segments);

		Vector3 point =
		{
			mLastCastStart.x + (mLastCastEnd.x - mLastCastStart.x) * t,
			mLastCastStart.y + (mLastCastEnd.y - mLastCastStart.y) * t,
			mLastCastStart.z + (mLastCastEnd.z - mLastCastStart.z) * t
		};

		const float sag = sinf(t * 3.14159f) * sagAmount;
		point.y -= sag;

		SimpleDraw::AddLine(previous, point, lineColor);

		previous = point;
	}

	// Lure / bobber
	SimpleDraw::AddSphere(12, 12, 0.25f, Colors::Red, mLastCastEnd);

	// Catch radius indicator around the lure
	const int circleSegments = 32;

	Vector3 circleCenter = mLastCastEnd;
	circleCenter.y += 0.05f;

	Vector3 previousPoint =
	{
		circleCenter.x + mCatchRadius,
		circleCenter.y,
		circleCenter.z
	};

	for (int i = 1; i <= circleSegments; ++i)
	{
		const float angle =
			(static_cast<float>(i) / static_cast<float>(circleSegments)) * 6.28318f;

		Vector3 currentPoint =
		{
			circleCenter.x + cosf(angle) * mCatchRadius,
			circleCenter.y,
			circleCenter.z + sinf(angle) * mCatchRadius
		};

		SimpleDraw::AddLine(previousPoint, currentPoint, Colors::Cyan);

		previousPoint = currentPoint;
	}
}

void FishingComponent::CastLine()
{
	if (mTransform->position.z < mMinWaterZ)
	{
		mLastCastCaughtFish = false;
		return;
	} 

	if (mCooldownTimer > 0.0f || mTransform == nullptr)
	{
		return;
	}

	Matrix4 matrix = mTransform->GetMatrix4();
	Vector3 forward = GetLook(matrix);
	forward = Normalize({ forward.x, 0.0f, forward.z });

	mLastCastStart = mTransform->position;
	mLastCastStart.y += 0.5f;

	const int mouseY = InputSystem::Get()->GetMouseScreenY();
	const float screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

	float mousePercent = 1.0f - (static_cast<float>(mouseY) / screenHeight);
	mousePercent = std::clamp(mousePercent, 0.0f, 1.0f);

	const float castDistance =
		mMinCastRange + ((mMaxCastRange - mMinCastRange) * mousePercent);

	mLastCastEnd = mLastCastStart + forward * castDistance;

	mCastLineTimer = 0.5f;
	mCooldownTimer = mCooldown;
	mLastCastCaughtFish = false;

	FishService* fishService = GetOwner().GetWorld().GetService<FishService>();
	if (fishService == nullptr)
	{
		return;
	}

	FishComponent* fish = fishService->FindFishNearLine(
		mLastCastStart,
		mLastCastEnd,
		mCatchRadius);

	if (fish != nullptr)
	{
		mHookedFish = fish;
		mCastLineTimer = 0.0f;
		mState = FishingState::ReelWindow;
		mStateTimer = 2.0f;
		mLastCastCaughtFish = true;
	}
}