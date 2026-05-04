#include "Precompiled.h"
#include "GameObjectFactory.h"
#include "GameObject.h"

#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "AnimatorComponent.h"
#include "RigidBodyComponent.h"

using namespace DgEngine;

namespace
{
	Component* AddComponent(const std::string& componentName, GameObject& gameObject)
	{
		Component* newComponent = nullptr;
		if (componentName == "TransformComponent")
		{
			newComponent = gameObject.AddComponent<TransformComponent>();
		}
		else if (componentName == "CameraComponent")
		{
			newComponent = gameObject.AddComponent<CameraComponent>();
		}
		else if (componentName == "FPSCameraComponent")
		{
			newComponent = gameObject.AddComponent<FPSCameraComponent>();
		}
		else if (componentName == "MeshComponent")
		{
			newComponent = gameObject.AddComponent<MeshComponent>();
		}
		else if (componentName == "ModelComponent")
		{
			newComponent = gameObject.AddComponent<ModelComponent>();
		}
		else if (componentName == "AnimatorComponent")
		{
			newComponent = gameObject.AddComponent<AnimatorComponent>();
		}
		else if (componentName == "RigidBodyComponent")
		{
			newComponent = gameObject.AddComponent<RigidBodyComponent>();
		}
		ASSERT(newComponent != nullptr, "GameObjectFactory: component type [%s] not found.", componentName.c_str());
		return newComponent;
	}
}

void GameObjectFactory::Make(const std::filesystem::path& templatePath, GameObject& gameObject, GameWorld& gameWorld)
{
	FILE* file = nullptr;
	auto err = fopen_s(&file, templatePath.u8string().c_str(), "r");
	ASSERT(err == 0, "GameObjectFactory: failed to open file %s", templatePath.u8string().c_str());

	char readBuffer[65536];
	rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));
	fclose(file);

	rapidjson::Document doc;
	doc.ParseStream(readStream);
	auto components = doc["Components"].GetObj();
	for (auto& component : components)
	{
		// todo read data
		Component* newComponent = AddComponent(component.name.GetString(), gameObject);
		if (newComponent != nullptr)
		{
			newComponent->Deserialize(component.value);
		}
	}
}