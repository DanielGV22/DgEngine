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

	DgEngine::Graphics::RenderObject mEarth;
	DgEngine::Graphics::RenderObject mMars;
	DgEngine::Graphics::StandardEffect mStandardEffect;

	DgEngine::Graphics::Animation mAnimation;
	float mAnimationTime = 0.0f;

};
 


