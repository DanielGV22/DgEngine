#include "GameState.h"
#include "CustomDebugDrawComponent.h"
#include "CustomDebugDrawService.h"

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
	return nullptr;
}

Component* GetCustomComponent(const std::string& componentName, GameObject& gameObject)
{
	if (componentName == "CustomDebugDrawComponent")
	{
		return gameObject.GetComponent<CustomDebugDrawComponent>();
	}
	return nullptr;
}

void GameState::Initialize()
{
	mLevelFile = L"../../Assets/Templates/Levels/level_2.json";

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

void GameState::SpawnPhysicsBenchmark(int count)
{
	int width = static_cast<int>(sqrt(static_cast<float>(count)));

	for (int i = 0; i < count; ++i)
	{
		int x = i % width;
		int z = i / width;

		GameObject* obj = mGameWorld.CreateGameObject(
			"BenchmarkSphere_" + std::to_string(i),
			"../../Assets/Templates/Objects/mesh_obj.json");

		auto transform =
			obj->GetComponent<TransformComponent>();

		if (transform != nullptr)
		{
			transform->position =
			{
				static_cast<float>(x * 3),
				15.0f + static_cast<float>(z / 10),
				static_cast<float>(z * 3)
			};
		}

		obj->Initialize();
	}
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
	ImGui::Separator();
	ImGui::Text("VGP340 Benchmark Spawner");

	if (ImGui::Button("Spawn 100 Physics Spheres"))
	{
		SpawnPhysicsBenchmark(100);
	}

	if (ImGui::Button("Spawn 500 Physics Spheres"))
	{
		SpawnPhysicsBenchmark(500);
	}

	if (ImGui::Button("Spawn 1000 Physics Spheres"))
	{
		SpawnPhysicsBenchmark(1000);
	}
	ImGui::End();
}