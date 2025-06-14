//#pragma once
//
//#include"DgEngine/Inc/DgEngine.h"
//
//class GameState : public DgEngine::AppState 
//{
//public:
//	void Initialize() override;
//	void Terminate() override;
//	void Update(float deltaTime) override;
//	void Render() override;
//	void DebugUI() override;
//
//private:
//
//	void UpdateCamera(float deltaTime);
//	
//
//	DgEngine::Graphics::Camera mCamera;
//	DgEngine::Graphics::Camera mRenderTargetCamera;
//
//	// GPU communication
//	DgEngine::Graphics::SimpleTextureEffect mSimpleTextureEffect;
//
//	// render object
//	DgEngine::Graphics::SimpleTextureEffect::RenderData mObject0;
//	DgEngine::Graphics::SimpleTextureEffect::RenderData mObject1;
//
//	// render target
//	DgEngine::Graphics::RenderTarget mRenderTarget;
//
//};

// SolarSystemState.h
#pragma once

#include "DgEngine/Inc/DgEngine.h"

struct CelestialBody
{
    std::string name;
    float radius = 1.0f;
    float distance = 1.0f;
    float orbitSpeed = 1.0f;
    float rotationSpeed = 1.0f;
    float orbitAngle = 0.0f;
    float rotationAngle = 0.0f;
    DgEngine::Graphics::TextureId textureId;
    DgEngine::Graphics::SimpleTextureEffect::RenderData renderData;
    int parentIndex = -1; // -1 = orbit around sun
};

class SolarSystemState : public DgEngine::AppState
{
public:
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Render() override;
    void DebugUI() override;

private:
    void UpdateCamera(float deltaTime);
    void RenderOrbits();

    DgEngine::Graphics::Camera mCamera;
    DgEngine::Graphics::Camera mPlanetCamera;
    DgEngine::Graphics::SimpleTextureEffect mSimpleTextureEffect;
    DgEngine::Graphics::RenderTarget mRenderTarget;

    std::vector<CelestialBody> mBodies;
    int mSelectedPlanet = 0;
    bool mShowOrbits = true;
    float mGlobalOrbitSpeed = 1.0f;
    float mGlobalRotationSpeed = 1.0f;

    DgEngine::Graphics::SimpleTextureEffect::RenderData mSkySphere;
};
