#pragma once

#include "TypeIds.h"

namespace DgEngine
{
	class GameObject;

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		Component(const Component&) = delete;
		Component(const Component&&) = delete;
		Component& operator=(const Component&) = delete;
		Component& operator=(const Component&&) = delete;

		virtual void Initialize() {}
		virtual void Terminate() {}
		virtual void Update(float deltaTime) {}
		virtual void DebugUI() {}

		// will read in data, apply to the object
		virtual void Deserialize(const rapidjson::Value& value) {}
		// will write out data to a json document, which will be saved to a json file
		virtual void Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue) {}

		virtual uint32_t GetTypeId() const = 0;

		GameObject& GetOwner() { return *mOwner; }
		const GameObject& GetOwner() const { return *mOwner; }

	private:
		friend class GameObject;
		GameObject* mOwner = nullptr;
	};
}