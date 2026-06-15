#include "FishService.h"
#include "FishComponent.h"
#include <algorithm>

using namespace DgEngine;
using namespace DgEngine::Math;

void FishService::Register(FishComponent* fish)
{
	auto iter = std::find(mFish.begin(), mFish.end(), fish);
	if (iter == mFish.end())
	{
		mFish.push_back(fish);
	}
}

void FishService::Unregister(FishComponent* fish)
{
	auto iter = std::find(mFish.begin(), mFish.end(), fish);
	if (iter != mFish.end())
	{
		mFish.erase(iter);
	}
}

FishComponent* FishService::FindFishNearPoint(const Vector3& point, float radius)
{
	FishComponent* closestFish = nullptr;
	float closestDistanceSqr = radius * radius;

	for (FishComponent* fish : mFish)
	{
		if (fish == nullptr || fish->IsCaught())
		{
			continue;
		}

		const Vector3 fishPos = fish->GetPosition();
		const Vector3 offset = fishPos - point;
		const float distSqr = MagnitudeSqr(offset);

		if (distSqr <= closestDistanceSqr)
		{
			closestDistanceSqr = distSqr;
			closestFish = fish;
		}
	}

	return closestFish;
}

FishComponent* FishService::FindFishNearLine(const Vector3& start, const Vector3& end, float radius)
{
	FishComponent* closestFish = nullptr;
	float closestDistanceSqr = radius * radius;

	const Vector3 line = end - start;
	const float lineLengthSqr = MagnitudeSqr(line);

	if (lineLengthSqr <= 0.0001f)
	{
		return FindFishNearPoint(end, radius);
	}

	for (FishComponent* fish : mFish)
	{
		if (fish == nullptr || fish->IsCaught())
		{
			continue;
		}

		const Vector3 fishPos = fish->GetPosition();

		float t = Dot(fishPos - start, line) / lineLengthSqr;
		t = std::clamp(t, 0.0f, 1.0f);

		const Vector3 closestPoint = start + line * t;
		const Vector3 offset = fishPos - closestPoint;
		const float distSqr = MagnitudeSqr(offset);

		if (distSqr <= closestDistanceSqr)
		{
			closestDistanceSqr = distSqr;
			closestFish = fish;
		}
	}

	return closestFish;
}
