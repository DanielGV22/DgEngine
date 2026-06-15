#pragma once

#include "CustomTypeIds.h"
#include <DgEngine/Inc/DgEngine.h>

class FishComponent : public DgEngine::Component
{
public:
	SET_TYPE_ID(CustomComponentId::Fish);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void DebugUI() override;
	void Deserialize(const rapidjson::Value& value) override;

	void Catch();

	bool IsCaught() const { return mCaught; }
	DgEngine::Math::Vector3 GetPosition() const;

private:
	void PickNewTarget();

	DgEngine::TransformComponent* mTransform = nullptr;

	DgEngine::Math::Vector3 mStartPosition = DgEngine::Math::Vector3::Zero;
	DgEngine::Math::Vector3 mTargetPosition = DgEngine::Math::Vector3::Zero;

	float mSwimSpeed = 2.0f;
	float mWanderRadius = 5.0f;
	float mCatchValue = 10.0f;

	bool mCaught = false;
};