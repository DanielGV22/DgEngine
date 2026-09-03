#pragma once

#include "Component.h"

namespace DgEngine
{
	class RenderObjectComponent : public Component
	{
	public:
		void Initialize() override;
		void Terminate() override;

		void Deserialize(const rapidjson::Value& value) override;
		void Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue) override;

		bool CanCastShadow() const;

		virtual Graphics::ModelId GetModelId() const { return 0; }
		virtual const Graphics::Model& GetModel() const = 0;
	private:
		bool mCastShadow = true;

	};
}