#pragma once

#pragma once

#include "Component.h"

namespace DgEngine
{
	class CameraComponent;
	class TransformComponent;

	class TPSCameraComponent :public Component
	{
	public:
		SET_TYPE_ID(ComponentId::TPCamera);

		void Initialize() override;
		void LateUpdate(float deltaTime) override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;
		void Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue) override;

	private:
		const TransformComponent* mTransformComponent = nullptr;
		CameraComponent* mCameraComponent = nullptr;

		Math::Vector3 mOffset = Math::Vector3::Zero;
		float mSmoothingValue = 0.0f;
	};
}