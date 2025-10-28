#pragma once

#include"DgEngine/Inc/DgEngine.h"

class GameState : public DgEngine::AppState 
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	

	void UpdateCamera(float deltaTime);


	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::DirectionalLight mDirectionalLight;

	DgEngine::Graphics::RenderGroup mCharacter;
	DgEngine::Graphics::StandardEffect mStandardEffect;



};
 


