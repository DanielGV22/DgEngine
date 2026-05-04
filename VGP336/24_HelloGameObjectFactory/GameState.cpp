#include "GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;


void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize();

	GameObject* cameraGO = mGameWorld.CreateGameObject("Camera", L"../../Assets/Templates/Objects/fps_camera_obj.json");
	cameraGO->Initialize();

	GameObject* transformGO = mGameWorld.CreateGameObject("Transform", L"../../Assets/Templates/Objects/transform_obj.json");
	transformGO->Initialize();

	GameObject* playerGO = mGameWorld.CreateGameObject("Player");
	TransformComponent* playerTransform = playerGO->AddComponent<TransformComponent>();
	playerTransform->position.x = 2.0f;
	playerGO->Initialize();

	GameObject* weaponGo = mGameWorld.CreateGameObject("Weapon");
	TransformComponent* weaponTransform = weaponGo->AddComponent<TransformComponent>();
	weaponTransform->position.x = -2.0f;
	weaponGo->Initialize();

	GameObject* obstacleGO = mGameWorld.CreateGameObject("Obstacle");
	TransformComponent* obstacleTransform = obstacleGO->AddComponent<TransformComponent>();
	obstacleTransform->position = { 0.0f, 0.0f, 2.0f };
	obstacleGO->Initialize();

	GameObject* vehicleGO = mGameWorld.CreateGameObject("Vehicle");
	TransformComponent* vehicleTransform = vehicleGO->AddComponent<TransformComponent>();
	vehicleTransform->position = { 0.0f, 0.0f, -2.0f };
	vehicleGO->Initialize();
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