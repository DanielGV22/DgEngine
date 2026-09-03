#pragma once

#include "Component.h"

namespace DgEngine
{
	class CameraComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Camera);

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;
		void Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue) override;

		Graphics::Camera& GetCamera();
		const Graphics::Camera& GetCamera() const;

	private:
		Graphics::Camera mCamera;
	};
}