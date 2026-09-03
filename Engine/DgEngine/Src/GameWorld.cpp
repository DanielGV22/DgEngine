#include "Precompiled.h"
#include "GameWorld.h"
#include "GameObjectFactory.h"

#include "CameraService.h"
#include "RenderService.h"
#include "PhysicsService.h"
#include "NetworkService.h"
#include "UIRenderService.h"
#include "SaveUtil.h"

using namespace DgEngine;


namespace
{
	CustomService TryAddService;
}

void GameWorld::SetCustomService(CustomService customService)
{
	TryAddService = customService;
}

void GameWorld::Initialize(uint32_t capacity)
{
	ASSERT(!mInitialized, "GameWorld: is already initialized.");
	for (auto& service : mServices)
	{
		service->Initialize();
	}

	mGameObjectSlots.resize(capacity);
	mFreeSlots.resize(capacity);
	std::iota(mFreeSlots.begin(), mFreeSlots.end(), 0);

	mInitialized = true;
}

void GameWorld::Terminate()
{
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->Terminate();
			slot.gameObject.reset();
		}
	}

	mGameObjectSlots.clear();
	mFreeSlots.clear();
	mToBeDestroyed.clear();

	for (auto& service : mServices)
	{
		service->Terminate();
		service.reset();
	}
	mServices.clear();

	mInitialized = false;
}

void GameWorld::Update(float deltaTime)
{
	// game objects update
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->Update(deltaTime);
		}
	}
	// services update (eg: physics)
	for (auto& service : mServices)
	{
		service->Update(deltaTime);
	}
	// game objects late update (react to physics results, etc)
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->LateUpdate(deltaTime);
		}
	}
	ProcessDestroyList();
}

void GameWorld::Render()
{
	for (auto& service : mServices)
	{
		service->Render();
	}
}

void GameWorld::DebugUI()
{
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->DebugUI();
		}
	}
	for (auto& service : mServices)
	{
		service->DebugUI();
	}
}

GameObject* GameWorld::CreateGameObject(std::string name, const std::filesystem::path& templatePath)
{
	ASSERT(mInitialized, "GameWorld: is not initialized.");
	if (mFreeSlots.empty())
	{
		ASSERT(false, "GameWorld: no free slots available.");
		return nullptr;
	}

	const uint32_t freeSlot = mFreeSlots.back();
	mFreeSlots.pop_back();

	Slot& slot = mGameObjectSlots[freeSlot];
	slot.gameObject = std::make_unique<GameObject>();
	slot.gameObject->SetName(name);
	slot.gameObject->mHandle.mIndex = freeSlot;
	slot.gameObject->mHandle.mGeneration = slot.generation;
	slot.gameObject->mWorld = this;
	if (!templatePath.empty())
	{
		slot.gameObject->mTemplateFileName = templatePath;
		GameObjectFactory::Make(templatePath, *slot.gameObject, *this);
	}
	return slot.gameObject.get();
}

void GameWorld::DestroyGameObject(const GameObjectHandle& handle)
{
	if (!IsValid(handle))
	{
		return;
	}

	Slot& slot = mGameObjectSlots[handle.mIndex];
	++slot.generation;
	mToBeDestroyed.push_back(handle.mIndex);
}

void GameWorld::LoadLevel(const std::filesystem::path& levelFile)
{
	mLevelFileName = levelFile;

	FILE* file = nullptr;
	auto err = fopen_s(&file, levelFile.u8string().c_str(), "r");
	ASSERT(err == 0 && file != nullptr, "GameWorld: failed to open %s.", levelFile.u8string().c_str());

	char readBuffer[65536];
	rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));
	fclose(file);

	rapidjson::Document doc;
	doc.ParseStream(readStream);

	auto services = doc["Services"].GetObj();
	for (auto& service : services)
	{
		std::string serviceName = service.name.GetString();
		Service* newService = nullptr;
		if (serviceName == "CameraService")
		{
			newService = AddService<CameraService>();
		}
		else if (serviceName == "RenderService")
		{
			newService = AddService<RenderService>();
		}
		else if (serviceName == "PhysicsService")
		{
			newService = AddService<PhysicsService>();
		}
		else if (serviceName == "UIRenderService")
		{
			newService = AddService<UIRenderService>();
		}
		else if (serviceName == "NetworkService")
		{
			newService = AddService<NetworkService>();
		}
		else
		{
			newService = TryAddService(serviceName, *this);
		}

		ASSERT(newService != nullptr, "GameWorld: failed to add service %s.", serviceName.c_str());
		newService->Deserialize(service.value);
	}

	uint32_t capacity = static_cast<uint32_t>(doc["Capacity"].GetInt());
	Initialize(capacity);

	auto gameObjects = doc["GameObjects"].GetObj();
	for (auto& gameObject : gameObjects)
	{
		std::string name = gameObject.name.GetString();
		std::string templateFile = gameObject.value["Template"].GetString();
		GameObject* go = CreateGameObject(name, templateFile);
		GameObjectFactory::OverrideDeserialize(gameObject.value, *go);
		go->Initialize();
	}
}

void GameWorld::SaveLevel(const std::filesystem::path& levelFile)
{
	// create document
	rapidjson::Document writeDoc(rapidjson::kObjectType);

	// save capacity
	int capacity = mGameObjectSlots.size();
	SaveUtil::WriteInt("Capacity", capacity, writeDoc, writeDoc);

	// save services
	rapidjson::Value serviceValue(rapidjson::kObjectType);
	for (auto& service : mServices)
	{
		service->Serialize(writeDoc, serviceValue);
	}
	writeDoc.AddMember("Services", serviceValue, writeDoc.GetAllocator());

	char buffer[65536];
	// save game objects
	rapidjson::Value gameObjectsValue(rapidjson::kObjectType);
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			rapidjson::Document originalDoc(rapidjson::kObjectType);
			if (!slot.gameObject->mTemplateFileName.empty())
			{
				FILE* file = nullptr;
				auto err = fopen_s(&file, slot.gameObject->mTemplateFileName.u8string().c_str(), "r");
				ASSERT(err == 0 && file != nullptr, "GameWorld: failed to open template file %s.", slot.gameObject->mTemplateFileName.u8string().c_str());
				rapidjson::FileReadStream readStream(file, buffer, sizeof(buffer));
				fclose(file);

				originalDoc.ParseStream(readStream);
			}
			// create document for game object
			rapidjson::Document gameObjectDoc(rapidjson::kObjectType);

			// save template
			rapidjson::GenericStringRef<char> goTemplate(slot.gameObject->mTemplateFileName.u8string().c_str());
			gameObjectDoc.AddMember("Template", goTemplate, gameObjectDoc.GetAllocator());

			// save component overrides 
			GameObjectFactory::SerializeGameObject(gameObjectDoc, originalDoc, *slot.gameObject);
			rapidjson::GenericStringRef<char> goNameKey(slot.gameObject->GetName().c_str());

			// save objects to the level
			gameObjectsValue.AddMember(goNameKey, gameObjectDoc, writeDoc.GetAllocator());
		}
	}
	writeDoc.AddMember("GameObjects", gameObjectsValue, writeDoc.GetAllocator());

	FILE* file = nullptr;
	auto err = fopen_s(&file, mLevelFileName.u8string().c_str(), "w");
	ASSERT(err == 0 && file != nullptr, "GameWorld: failed to open file to save %s", mLevelFileName.u8string().c_str());
	rapidjson::FileWriteStream writeStream(file, buffer, sizeof(buffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(writeStream);
	writeDoc.Accept(writer);
	fclose(file);

}


bool GameWorld::IsValid(const GameObjectHandle& handle)
{
	if (handle.mIndex < 0 || handle.mIndex >= mGameObjectSlots.size())
	{
		return false;
	}
	if (mGameObjectSlots[handle.mIndex].generation != handle.mGeneration)
	{
		return false;
	}
	return true;
}

void GameWorld::ProcessDestroyList()
{
	for (uint32_t index : mToBeDestroyed)
	{
		Slot& slot = mGameObjectSlots[index];
		GameObject* gameObject = slot.gameObject.get();
		ASSERT(!IsValid(gameObject->GetHandle()), "GameWorld: gameObjects is still alive.");

		gameObject->Terminate();
		slot.gameObject.reset();
		mFreeSlots.push_back(index);
	}
	mToBeDestroyed.clear();
}