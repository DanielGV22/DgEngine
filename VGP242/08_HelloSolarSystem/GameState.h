

#pragma once

#include "DgEngine/Inc/DgEngine.h"
#include <array>

class GameState : public DgEngine::AppState
{
public:
    static constexpr int PlanetCount = 9;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Render() override;
    void DebugUI() override;

private:
    struct Object
    {
        DgEngine::Math::Matrix4 matWorld = DgEngine::Math::Matrix4::Identity;
        DgEngine::Graphics::MeshBuffer meshBuffer;
        DgEngine::Graphics::TextureId textureId = 0;
    };

    struct Celestial : public Object
    {
        float orbitRadius = 0.0f;
        float orbitSpeed = 0.0f;
        float rotationSpeed = 0.0f;
        float orbitAngle = 0.0f;
        float rotationAngle = 0.0f;
        float scale = 1.0f;
    };

    DgEngine::Graphics::Camera mCamera;
    DgEngine::Graphics::Camera mRenderTargetCamera;
    DgEngine::Graphics::ConstantBuffer mTransformBuffer;
    DgEngine::Graphics::VertexShader mVertexShader;
    DgEngine::Graphics::PixelShader mPixelShader;
    DgEngine::Graphics::Sampler mSampler;

    Object mSkySphere;
    Object mSun;
    std::array<Celestial, PlanetCount> mPlanets;
    Celestial mMoon;

    DgEngine::Graphics::RenderTarget mRenderTarget;

    bool mShowRings = true;
    float mGlobalSpeed = 1.0f;
    int mSelectedPlanet = 0;

    void UpdateCamera(float dt);
    void RenderObject(const Object& obj, const DgEngine::Graphics::Camera& cam);
};