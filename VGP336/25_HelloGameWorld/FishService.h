#pragma once

#include "CustomTypeIds.h"
#include <DgEngine/Inc/DgEngine.h>

class FishComponent;

class FishService : public DgEngine::Service
{
public:
	SET_TYPE_ID(CustomServiceId::Fish);

	void Register(FishComponent* fish);
	void Unregister(FishComponent* fish);

	FishComponent* FindFishNearPoint(const DgEngine::Math::Vector3& point, float radius);
	FishComponent* FindFishNearLine(
		const DgEngine::Math::Vector3& start,
		const DgEngine::Math::Vector3& end,
		float radius);

private:
	std::vector<FishComponent*> mFish;
};