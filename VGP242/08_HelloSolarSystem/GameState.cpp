
#include "GameState.h"
#include <ImGui/Inc/imgui.h>
#include <array>
#include <filesystem>
#include <cmath>

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Math;
using namespace DgEngine::Input;

static const std::array<const wchar_t*, GameState::PlanetCount> PlanetTextures = {
    L"mercury.jpg", L"venus.jpg", L"earth.jpg", L"mars.jpg",
    L"jupiter.jpg", L"saturn.jpg", L"uranus.jpg", L"neptune.jpg", L"pluto.jpg"
};
static const std::array<const char*, GameState::PlanetCount> PlanetNames = {
    "Mercury","Venus","Earth","Mars",
    "Jupiter","Saturn","Uranus","Neptune","Pluto"
};
static const std::array<float, GameState::PlanetCount> PlanetScales = {
    0.383f, 0.949f, 1.0f, 0.532f,
    10.5f, 9.45f, 4.01f, 3.88f, 0.186f
};
static constexpr float MaxPlanetScale = 10.50f; // largest planet scale (Jupiter)

void GameState::Initialize()
{
    // camera
    mCamera.SetPosition({ 0, 0, -20 });
    mCamera.SetLookAt(Vector3::Zero);
    mRenderTargetCamera = mCamera;
    mRenderTargetCamera.SetAspectRatio(1.0f);

    // shaders
    auto fx = std::filesystem::path{ L"../../Assets/Shaders/DoTexture.fx" };
    mVertexShader.Initialize<VertexPX>(fx);
    mPixelShader.Initialize(fx);
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
    mTransformBuffer.Initialize(sizeof(Matrix4));

    // meshes
    MeshPX skyMesh = MeshBuilder::CreateSkySpherePX(60, 60, 100.0f);
    mSkySphere.meshBuffer.Initialize(skyMesh);
    mSkySphere.textureId = TextureManager::Get()->LoadTexture(L"space.jpg");

    // Sun mesh
    MeshPX sunMesh = MeshBuilder::CreateSpherePX(60, 60, 6.0f);
    mSun.meshBuffer.Initialize(sunMesh);
    mSun.textureId = TextureManager::Get()->LoadTexture(L"sun.jpg");

    for (int i = 0; i < PlanetCount; ++i)
    {
        auto& p = mPlanets[i];
        // Largest planet scale
        constexpr float sunRadius = 6.0f; 
        float maxPlanetRadius = sunRadius * 0.5f;
        // relative size 
        float frac = PlanetScales[i] / MaxPlanetScale;
        float scaled = frac * maxPlanetRadius;
        // smaller planets at least half the big ones
        float minScale = maxPlanetRadius * 0.25f;
        p.scale = std::max(scaled, minScale);
        MeshPX planetMesh = MeshBuilder::CreateSpherePX(60, 60, p.scale);
        p.meshBuffer.Initialize(planetMesh);
        p.textureId = TextureManager::Get()->LoadTexture(PlanetTextures[i]);
        p.orbitRadius = 5.0f + i * 3.0f;
        p.orbitSpeed = 0.2f + 0.05f * i;
        p.rotationSpeed = 1.0f + 0.2f * i;
    }

    // Moon scale relative to Earth's
    {
        constexpr float sunRadius = 6.0f;
        float maxPlanetRadius = sunRadius * 0.5f;
        mMoon.scale = maxPlanetRadius * 0.4f;
    }
    MeshPX moonMesh = MeshBuilder::CreateSpherePX(60, 60, mMoon.scale);
    mMoon.meshBuffer.Initialize(moonMesh);
    mMoon.textureId = TextureManager::Get()->LoadTexture(L"pluto.jpg");

    mRenderTarget.Initialize(512, 512, RenderTarget::Format::RGBA_U32);
}

void GameState::Terminate()
{
    mRenderTarget.Terminate();
    auto release = [&](Object& obj)
        {
            TextureManager::Get()->ReleaseTexture(obj.textureId);
            obj.meshBuffer.Terminate();
        };
    release(mSkySphere);
    release(mSun);
    for (auto& p : mPlanets) release(p);
    release(mMoon);
    mTransformBuffer.Terminate();
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
}

void GameState::Update(float dt)
{
    UpdateCamera(dt);
    for (int i = 0; i < PlanetCount; ++i)
    {
        auto& p = mPlanets[i];
        p.orbitAngle += p.orbitSpeed * dt * mGlobalSpeed;
        p.rotationAngle += p.rotationSpeed * dt * mGlobalSpeed;
        float x = std::cos(p.orbitAngle) * p.orbitRadius;
        float z = std::sin(p.orbitAngle) * p.orbitRadius;
        p.matWorld = Matrix4::RotationY(p.rotationAngle) * Matrix4::Translation({ x, 0, z });
    }
    {
        const auto& earth = mPlanets[2];
        Vector3 pos = GetTranslation(earth.matWorld);
        float mx = std::cos(earth.orbitAngle * 4.0f) * (earth.scale + 0.5f);
        float mz = std::sin(earth.orbitAngle * 4.0f) * (earth.scale + 0.5f);
        mMoon.matWorld = Matrix4::Translation({ pos.x + mx, 0, pos.z + mz });
    }
    if (mSelectedPlanet >= 0 && mSelectedPlanet < PlanetCount)
    {
        Vector3 f = GetTranslation(mPlanets[mSelectedPlanet].matWorld);
        mRenderTargetCamera.SetLookAt(f);
    }
}

void GameState::Render()
{
    RenderObject(mSkySphere, mCamera);
    if (mShowRings)
    {
        for (int i = 0; i < PlanetCount; ++i)
            SimpleDraw::AddGroundCircle(120, mPlanets[i].orbitRadius, Colors::Gray, Vector3::Zero);
    }
    SimpleDraw::Render(mCamera);

    mRenderTarget.BeginRender();
    if (mSelectedPlanet >= 0 && mSelectedPlanet < PlanetCount)
    {
        const Celestial& cp = mPlanets[mSelectedPlanet];
        Object pCopy;
        pCopy.meshBuffer = cp.meshBuffer;
        pCopy.textureId = cp.textureId;
        if (PlanetScales[mSelectedPlanet] < 6)
        {
            float previewScale = cp.scale * 2.0f;
            pCopy.matWorld = Matrix4::Scaling(previewScale);
            RenderObject(pCopy, mRenderTargetCamera);
        }
        else {
            float previewScale = cp.scale * .5f;
            pCopy.matWorld = Matrix4::Scaling(previewScale);
            RenderObject(pCopy, mRenderTargetCamera);
        }
    }
    mRenderTarget.EndRender();

    RenderObject(mSun, mCamera);
    for (auto& p : mPlanets) RenderObject(p, mCamera);
    RenderObject(mMoon, mCamera);
}

void GameState::RenderObject(const Object& obj, const Graphics::Camera& cam)
{
    Matrix4 wvp = Transpose(obj.matWorld * cam.GetViewMatrix() * cam.GetProjectionMatrix());
    mTransformBuffer.Update(&wvp);
    mVertexShader.Bind();
    mPixelShader.Bind();
    mSampler.BindPS(0);
    mTransformBuffer.BindVS(0);
    TextureManager::Get()->BindPS(obj.textureId, 0);
    obj.meshBuffer.Render();
}

void GameState::DebugUI()
{
    ImGui::Begin("Solar System Controls");
    ImGui::Checkbox("Show Orbit Rings", &mShowRings);
    ImGui::SliderFloat("Global Speed", &mGlobalSpeed, 0.1f, 5.0f);
    ImGui::Combo("Focus Planet", &mSelectedPlanet, PlanetNames.data(), PlanetCount);
    if (mSelectedPlanet >= 0 && mSelectedPlanet < PlanetCount)
    {
        auto& p = mPlanets[mSelectedPlanet];
        ImGui::Text(PlanetNames[mSelectedPlanet]);
        ImGui::SliderFloat("Orbit Speed", &p.orbitSpeed, 0.0f, 2.0f);
        ImGui::SliderFloat("Rotation Speed", &p.rotationSpeed, 0.0f, 5.0f);
    }
    ImGui::Text("Preview:"); ImGui::Image(mRenderTarget.GetRawData(), { 256,256 });
    ImGui::End();
}

void GameState::UpdateCamera(float dt)
{
    auto in = InputSystem::Get();
    float speed = in->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
    float turn = 0.1f;
    if (in->IsKeyDown(KeyCode::W)) mCamera.Walk(speed * dt);
    if (in->IsKeyDown(KeyCode::S)) mCamera.Walk(-speed * dt);
    if (in->IsKeyDown(KeyCode::D)) mCamera.Strafe(speed * dt);
    if (in->IsKeyDown(KeyCode::A)) mCamera.Strafe(-speed * dt);
    if (in->IsKeyDown(KeyCode::E)) mCamera.Rise(speed * dt);
    if (in->IsKeyDown(KeyCode::Q)) mCamera.Rise(-speed * dt);
    if (in->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(in->GetMouseMoveX() * turn * dt);
        mCamera.Pitch(in->GetMouseMoveY() * turn * dt);
    }
}

