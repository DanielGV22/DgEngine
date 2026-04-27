#include "GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;


void GameState::Initialize()
{
	mGameWorld.Initialize();

	GameObject* transformGO = mGameWorld.CreateGameObject("Transform");
	transformGO->AddComponent<TransformComponent>();
	transformGO->Initialize();

	GameObject* cameraGO = mGameWorld.CreateGameObject("Camera");
	mCameraComponent = cameraGO->AddComponent<CameraComponent>();
	cameraGO->AddComponent<FPSCameraComponent>();
	cameraGO->Initialize();

	GameObject* playerGO = mGameWorld.CreateGameObject("Player");
	TransformComponent* playerTransform = playerGO->AddComponent<TransformComponent>();
	playerTransform->position.x = 2.0f;
	playerGO->Initialize();
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

	SimpleDraw::Render(mCameraComponent->GetCamera());
}