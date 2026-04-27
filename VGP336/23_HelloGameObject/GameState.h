#pragma once

#include <DgEngine/Inc/DgEngine.h>

class GameState : public DgEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	DgEngine::GameWorld mGameWorld;
	DgEngine::CameraComponent* mCameraComponent = nullptr;
};