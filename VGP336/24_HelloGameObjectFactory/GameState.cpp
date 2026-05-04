#include "GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;


void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.AddService<PhysicsService>();
	mGameWorld.Initialize();

	std::vector<GameObject*> gameObjects;
	gameObjects.push_back(mGameWorld.CreateGameObject("Transform", L"../../Assets/Templates/Objects/transform_obj.json"));
	gameObjects.push_back(mGameWorld.CreateGameObject("Camera", L"../../Assets/Templates/Objects/fps_camera_obj.json"));

	gameObjects.push_back(mGameWorld.CreateGameObject("Player", L"../../Assets/Templates/Objects/transform_obj.json"));
	TransformComponent* playerTransform = gameObjects.back()->GetComponent<TransformComponent>();

	gameObjects.push_back(mGameWorld.CreateGameObject("SphereObj", L"../../Assets/Templates/Objects/mesh_obj.json"));
	TransformComponent* meshTransform = gameObjects.back()->GetComponent<TransformComponent>();
	meshTransform->position = { -2.0f, 0.0f, 0.0f };

	gameObjects.push_back(mGameWorld.CreateGameObject("ModelObj", L"../../Assets/Templates/Objects/model_obj.json"));
	TransformComponent* modelTransform = gameObjects.back()->GetComponent<TransformComponent>();
	modelTransform->position = { 0.0f, 0.0f, 2.0f };

	gameObjects.push_back(mGameWorld.CreateGameObject("Ground", L"../../Assets/Templates/Objects/ground.json"));

	for (GameObject* go : gameObjects)
	{
		go->Initialize();
	}
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
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	mGameWorld.DebugUI();
	ImGui::End();

}