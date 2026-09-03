#include "Precompiled.h"
#include "TransformComponent.h"
#include "SaveUtil.h"
#include "GameObject.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

void TransformComponent::DebugUI()
{
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	ImGui::DragFloat4("Rotation", &rotation.x, 0.0001f);
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);

	SimpleDraw::AddTransform(GetMatrix4());
}

void TransformComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadVector3("Position", position, value);
	SaveUtil::ReadQuaternion("Rotation", rotation, value);
	SaveUtil::ReadVector3("Scale", scale, value);
}

void TransformComponent::Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue)
{
	rapidjson::Value componentValue(rapidjson::kObjectType);
	// compare with original, if different, save current value
	value.AddMember("TransformComponent", componentValue, doc.GetAllocator());
}

Transform TransformComponent::GetWorldTransform() const
{
	Transform worldTransform = *this;
	const GameObject* parent = GetOwner().GetParent();
	if (parent != nullptr)
	{
		Math::Matrix4 matWorld = GetMatrix4();
		while (parent != nullptr)
		{
			const TransformComponent* transformComponent = parent->GetComponent<TransformComponent>();
			ASSERT(transformComponent != nullptr, "TransformComponent: parent game object doesn't have transform component");
			matWorld = matWorld * transformComponent->GetMatrix4();
			parent = parent->GetParent();
		}
		worldTransform.position = Math::GetTranslation(matWorld);
		worldTransform.rotation = Math::Quaternion::CreateFromRotationMatrix(matWorld);
		worldTransform.scale = Math::GetScale(matWorld);
	}
	return worldTransform;
}