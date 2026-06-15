#include "GameState.h"
#include "CustomDebugDrawComponent.h"
#include "CustomDebugDrawService.h"
#include "FishService.h"
#include "FishComponent.h"
#include "FishingComponent.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;

Service* MakeCustomService(const std::string& serviceName, GameWorld& gameWorld)
{
	if (serviceName == "CustomDebugDrawService")
	{
		return gameWorld.AddService<CustomDebugDrawService>();
	}

	if (serviceName == "FishService")
	{
		return gameWorld.AddService<FishService>();
	}
	// add another
	// add text
	return nullptr;
}

Component* MakeCustomComponent(const std::string& componentName, GameObject& gameObject)
{
	if (componentName == "CustomDebugDrawComponent")
	{
		return gameObject.AddComponent<CustomDebugDrawComponent>();
	}

	if (componentName == "FishComponent")
	{
		return gameObject.AddComponent<FishComponent>();
	}

	if (componentName == "FishingComponent")
	{
		return gameObject.AddComponent<FishingComponent>();
	}
	return nullptr;
}

Component* GetCustomComponent(const std::string& componentName, GameObject& gameObject)
{
	if (componentName == "CustomDebugDrawComponent")
	{
		return gameObject.GetComponent<CustomDebugDrawComponent>();
	}

	if (componentName == "FishComponent")
	{
		return gameObject.GetComponent<FishComponent>();
	}

	if (componentName == "FishingComponent")
	{
		return gameObject.GetComponent<FishingComponent>();
	}
	return nullptr;
}

void GameState::Initialize()
{
	mLevelFile = L"../../Assets/Templates/Levels/level.json";

	// set a callback to try make a custom service (any service that is NoT part of the engine and unique to the project)
	GameWorld::SetCustomService(MakeCustomService);
	// sets callbacks to try make/get a custom component (any component that is NOT part of the engine)
	GameObjectFactory::SetCustomMake(MakeCustomComponent);
	GameObjectFactory::SetCustomGet(GetCustomComponent);

	mGameWorld.LoadLevel(mLevelFile);


}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();

	UIFont::Get()->DrawString(L"Hello Game World!",{300.0f,30.0f}, Colors::White, 64.0f);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	mGameWorld.DebugUI();
	if (ImGui::Button("ReloadLevel"))
	{
		mGameWorld.Terminate();
		mGameWorld.LoadLevel(mLevelFile);
	}
	ImGui::End();

}