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

	DgEngine::Graphics::RenderTarget mSceneRenderTarget;
	DgEngine::Graphics::VisionEffect mVisionEffect;

	int   mVisionMode = 0;   // 0 = off, 1 = thermal, 2 = night
	float mTime = 0.0f;

	DgEngine::Graphics::CRTEffect mCRTEffect;
	int mPostProcessMode = 0; // 0 = none, 1 = vision, 2 = CRT

	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::DirectionalLight mDirectionalLight;

	DgEngine::Graphics::RenderGroup mCharacter;
	DgEngine::Graphics::RenderGroup mCharacter02;
	DgEngine::Graphics::RenderGroup mCharacter03;
	DgEngine::Graphics::RenderObject mGround;

	DgEngine::Graphics::Terrain mTerrain;

	DgEngine::Graphics::RenderObject mScreenQuad;

	DgEngine::Graphics::StandardEffect mStandardEffect;
	DgEngine::Graphics::ShadowEffect mShadowEffect;
	DgEngine::Graphics::TerrainEffect mTerrainEffect;

};
 


