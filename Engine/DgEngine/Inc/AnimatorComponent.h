#pragma once

#include "Component.h"

namespace DgEngine
{
	class AnimatorComponent : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Animator);

		void Initialize() override;
		void Update(float deltaTime) override;
		void DebugUI() override;
		void Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue) override;

		bool Play(int index, bool looping = false);

		Graphics::Animator& GetAnimator();
		const Graphics::Animator& GetAnimator() const;

	private:
		Graphics::Animator mAnimator;
	};
}