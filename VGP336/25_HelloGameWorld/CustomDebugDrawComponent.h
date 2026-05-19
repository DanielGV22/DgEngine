#pragma once

#include "CustomTypeIds.h"
#include <DgEngine/Inc/DgEngine.h>

class CustomDebugDrawComponent : public DgEngine::Component
{
public:
	SET_TYPE_ID(CustomComponentId::CustomDebugDraw);

	void Initialize() override;
	void Terminate() override;
	void DebugUI() override;
	void Deserialize(const rapidjson::Value& value) override;

	void AddDebugDraw() const;
private:
	const DgEngine::TransformComponent* mTransformComponent = nullptr;
	DgEngine::Math::Vector3 mPosition = DgEngine::Math::Vector3::Zero;
	DgEngine::Graphics::Color mColor = DgEngine::Graphics::Colors::White;

	uint32_t mSlices = 0;
	uint32_t mRings = 0;
	float mRadius = 0;
};