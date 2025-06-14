//#include"GameState.h"
//
//using namespace DgEngine;
//using namespace DgEngine::Graphics;
//using namespace DgEngine::Input;
//
//void GameState::Initialize()
//{
//    mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
//    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
//
//    mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
//    mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
//    mRenderTargetCamera.SetAspectRatio(1.0f);
//
//    // initialize gpu communication
//	mSimpleTextureEffect.Initialize();
//
//    // initialize render object
//    MeshPX sphere = MeshBuilder::CreateSpherePX(60, 60, 1.0f);
//    mObject0.mesh.Initialize(sphere);
//    mObject0.textureId = TextureManager::Get()->LoadTexture(L"sun.jpg");
//
//   // mObject1.mesh.Initialize(sphere);
//
//    MeshPX thunderbird = MeshBuilder::CreateObjPX(L"../../Assets/Models/Thunderbird/Thunderbird.obj", 0.01f);
//    mObject1.mesh.Initialize(thunderbird);
//    mObject1.textureId = TextureManager::Get()->LoadTexture(L"../../Assets/Models/Thunderbird/space.jpg",false);
//    //mObject1.textureId = TextureManager::Get()->LoadTexture(L"earth.jpg");
//    mObject1.matWorld = Math::Matrix4::Translation({ 0.0f, 1.0f, 0.0f });
//
//
//    constexpr uint32_t size = 512;
//    mRenderTarget.Initialize(size, size, RenderTarget::Format::RGBA_U32);
//}
//void GameState::Terminate()
//{
//    mRenderTarget.Terminate();
//    TextureManager::Get()->ReleaseTexture(mObject0.textureId);
//    TextureManager::Get()->ReleaseTexture(mObject1.textureId);
//    mObject0.mesh.Terminate();
//    mObject1.mesh.Terminate();
//	mSimpleTextureEffect.Terminate();
//}
//void GameState::Update(float deltaTime)
//{
//    UpdateCamera(deltaTime);
//}
//
//void GameState::Render()
//{
//    SimpleDraw::AddGroundPlane(10.0f, Colors::DarkGray);
//    SimpleDraw::Render(mCamera);
//
//    // render to the render target
//	mSimpleTextureEffect.SetCamera(mRenderTargetCamera);
//    mRenderTarget.BeginRender();
//	    mSimpleTextureEffect.Begin();
//	              mSimpleTextureEffect.Render(mObject0);
//	              mSimpleTextureEffect.Render(mObject1);
//        mSimpleTextureEffect.End();
//    mRenderTarget.EndRender();
//
//    // render to the scene
//    mSimpleTextureEffect.SetCamera(mCamera);
//    mSimpleTextureEffect.Begin();
//           mSimpleTextureEffect.Render(mObject0);
//           mSimpleTextureEffect.Render(mObject1);
//    mSimpleTextureEffect.End();
//
//}
//
//bool gInvertValue = false;
//float gFloatVal = 0.0f;
//Math::Vector3 gV0 = Math::Vector3::Zero;
//Math::Vector3 gV1 = Math::Vector3::One;
//Math::Vector3 gV2 = Math::Vector3::XAxis;
//Color gDisplayColor = Colors::White;
//Color gColor = Colors::White;
//
//enum class Shape
//{
//    None,
//    AABB,
//    AABBFilled,
//    Sphere,
//    GroundPlane,
//    GroundCircle,
//    Transform
//};
//const char* gShapeNames[] =
//{
//    "None",
//    "AABB",
//    "AABBFilled",
//    "Sphere",
//    "GroundPlane",
//    "GroundCircle",
//    "Transform"
//};
//
//Shape gCurrentShape = Shape::None;
//void GameState::DebugUI()
//{
//    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
//    ImGui::Text("Hello Yall");
//    ImGui::Checkbox("Invert Color", &gInvertValue);
//    ImGui::DragFloat("Float", &gFloatVal);
//    ImGui::DragFloat3("V0", &gV0.x, 0.1f);
//    ImGui::DragFloat3("V1", &gV1.x, 0.1f);
//    ImGui::DragFloat3("V2", &gV2.x, 0.1f);
//    ImGui::ColorEdit4("Color", &gColor.r);
//
//    if (gInvertValue)
//    {
//        gDisplayColor = Color(1.0f - gColor.r, 1.0f - gColor.g, 1.0f - gColor.b, gColor.a);
//    }
//    else
//    {
//        gDisplayColor = gColor;
//    }
//
//    int currentShape = (int)gCurrentShape;
//    if (ImGui::Combo("Shape", &currentShape, gShapeNames, std::size(gShapeNames)))
//    {
//        gCurrentShape = (Shape)currentShape;
//    }
//
//
//    switch (gCurrentShape)
//    {
//    case Shape::None: break;
//    case Shape::AABB:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddAABB(gV0 + gV2, gV1 + gV2, gDisplayColor);
//        break;
//    }
//    case Shape::AABBFilled:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddFilledAABB(gV0 + gV2, gV1 + gV2, gDisplayColor);
//        break;
//    }
//    case Shape::Sphere:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddSphere(60, 60, gFloatVal, gDisplayColor, gV0 + gV2);
//        break;
//    }
//    case Shape::GroundPlane:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddGroundPlane(gFloatVal, gDisplayColor);
//        break;
//    }
//    case Shape::GroundCircle:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddGroundCircle(60, gFloatVal, gDisplayColor, gV0 + gV2);
//        break;
//    }
//    case Shape::Transform:
//    {
//        // ImGui::DragFloat("Min");
//        // ImGui::DragFloat("Max");
//        SimpleDraw::AddTransform(Math::Matrix4::Translation(gV2));
//        break;
//    }
//    }
//
//    ImGui::Separator();
//    ImGui::Text("RenderTarget");
//    ImGui::Image(
//        mRenderTarget.GetRawData(),
//        { 128, 128 },
//        { 0, 0 },
//        { 1, 1 },
//        { 1, 1, 1, 1 },
//        { 1, 1, 1, 1 }
//    );
//
//    ImGui::End();
//}
//
//
//void GameState::UpdateCamera(float deltaTime)
//{
//    Input::InputSystem* input = Input::InputSystem::Get();
//    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
//    const float turnSpeed = 0.1f;
//
//    if (input->IsKeyDown(KeyCode::W))
//    {
//        mCamera.Walk(moveSpeed * deltaTime);
//    }
//    if (input->IsKeyDown(KeyCode::S))
//    {
//        mCamera.Walk(-moveSpeed * deltaTime);
//    }
//    if (input->IsKeyDown(KeyCode::D))
//    {
//        mCamera.Strafe(moveSpeed * deltaTime);
//    }
//    if (input->IsKeyDown(KeyCode::A))
//    {
//        mCamera.Strafe(-moveSpeed * deltaTime);
//    }
//    if (input->IsKeyDown(KeyCode::E))
//    {
//        mCamera.Rise(moveSpeed * deltaTime);
//    }
//    if (input->IsKeyDown(KeyCode::Q))
//    {
//        mCamera.Rise(-moveSpeed * deltaTime);
//    }
//
//    if (input->IsMouseDown(MouseButton::RBUTTON))
//    {
//        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime); // INVERT IF U WANT
//        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
//    }
//}

// SolarSystemState.cpp
#include "GameState.h"
#include <ImGui/Inc/imgui.h>
#include <array>

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;

namespace
{
    constexpr std::array<const char*, 9> PlanetNames = {
        "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"
    };
    constexpr std::array<const wchar_t*, 10> PlanetTextures = {
        L"mercury.jpg",
        L"venus.jpg",
        L"earth.jpg",
        L"mars.jpg",
        L"jupiter.jpg",
        L"saturn.jpg",
        L"uranus.jpg",
        L"neptune.jpg",
        L"pluto.jpg",
        L"sun.jpg"
    };
}

void SolarSystemState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 50.0f, -150.0f });
    mCamera.SetLookAt({ 0.0f, 50.0f, 0.0f });
    mPlanetCamera.SetAspectRatio(1.0f);

    mSimpleTextureEffect.Initialize();
    mRenderTarget.Initialize(512, 512, RenderTarget::Format::RGBA_U32);

    auto* tm = TextureManager::Get();
    MeshPX sphere = MeshBuilder::CreateSpherePX(60, 60, 1.0f);

    // Sky sphere
    mSkySphere.mesh.Initialize(MeshBuilder::CreateSpherePX(30, 30, 500.0f));
    mSkySphere.textureId = tm->LoadTexture(L"space.jpg");

    // Sun
    CelestialBody sun;
    sun.name = "Sun";
    sun.radius = 10.0f;
    sun.textureId = tm->LoadTexture(PlanetTextures[9]);
    sun.renderData.mesh.Initialize(sphere);
    mBodies.push_back(sun);

    // Planets
    for (size_t i = 0; i < PlanetNames.size(); ++i)
    {
        CelestialBody body;
        body.name = PlanetNames[i];
        body.radius = 1.0f + static_cast<float>(i) * 0.3f;
        body.distance = 15.0f + static_cast<float>(i) * 10.0f;
        body.orbitSpeed = 0.2f + static_cast<float>(i) * 0.05f;
        body.rotationSpeed = 0.3f + static_cast<float>(i) * 0.1f;
        body.textureId = tm->LoadTexture(PlanetTextures[i]);
        body.renderData.mesh.Initialize(sphere);
        mBodies.push_back(body);
    }

    // Moon around Earth
    CelestialBody moon;
    moon.name = "Moon";
    moon.radius = 0.5f;
    moon.distance = 2.0f;
    moon.orbitSpeed = 1.5f;
    moon.rotationSpeed = 2.0f;
    moon.textureId = tm->LoadTexture(L"pluto.jpg");
    moon.renderData.mesh.Initialize(sphere);
    moon.parentIndex = 3; // Earth index
    mBodies.push_back(moon);
}

void SolarSystemState::Terminate()
{
    mSimpleTextureEffect.Terminate();
    mRenderTarget.Terminate();

    auto* tm = TextureManager::Get();
    for (auto& body : mBodies)
    {
        tm->ReleaseTexture(body.textureId);     //  Unload each texture
        body.renderData.mesh.Terminate();      //  Terminate the mesh
    }
    mBodies.clear();

    tm->ReleaseTexture(mSkySphere.textureId);   //  Unload sky sphere texture
    mSkySphere.mesh.Terminate();               //  Terminate the sky sphere mesh
}

void SolarSystemState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    for (auto& body : mBodies)
    {
        body.orbitAngle += deltaTime * body.orbitSpeed * mGlobalOrbitSpeed;
        body.rotationAngle += deltaTime * body.rotationSpeed * mGlobalRotationSpeed;
    }
}

void SolarSystemState::Render()
{
    mSimpleTextureEffect.SetCamera(mCamera);
    mSimpleTextureEffect.Begin();
    mSimpleTextureEffect.Render(mSkySphere);

    for (size_t i = 0; i < mBodies.size(); ++i)
    {
        auto& body = mBodies[i];
        Math::Vector3 position = { body.distance * cos(body.orbitAngle), 0.0f, body.distance * sin(body.orbitAngle) };
        if (body.parentIndex >= 0)
        {
            const auto& parent = mBodies[body.parentIndex];
            Math::Vector3 parentPos = { parent.distance * cos(parent.orbitAngle), 0.0f, parent.distance * sin(parent.orbitAngle) };
            position += parentPos;
        }
        Math::Matrix4 matTrans = Math::Matrix4::Translation(position);
        Math::Matrix4 matRot = Math::Matrix4::RotationY(body.rotationAngle);
        Math::Matrix4 matScale = Math::Matrix4::Scaling(body.radius);
        body.renderData.matWorld = matRot * matScale * matTrans;
        body.renderData.textureId = body.textureId;
        mSimpleTextureEffect.Render(body.renderData);
    }
    mSimpleTextureEffect.End();

    if (mShowOrbits)
        RenderOrbits();

    // Render selected planet view
    mPlanetCamera.SetLookAt(Math::Vector3::Zero);
    mPlanetCamera.SetPosition({ 0.0f, 0.0f, -15.0f });
    mSimpleTextureEffect.SetCamera(mPlanetCamera);

    const auto& selected = mBodies[mSelectedPlanet + 1];
    mRenderTarget.BeginRender();
    mSimpleTextureEffect.Begin();
    mSimpleTextureEffect.Render(selected.renderData);
    mSimpleTextureEffect.End();
    mRenderTarget.EndRender();
}

void SolarSystemState::RenderOrbits()
{
    for (const auto& body : mBodies)
    {
        if (body.name == "Sun") continue;
        float r = body.distance;
        SimpleDraw::AddGroundCircle(60, r, Colors::Gray, Math::Vector3::Zero);
    }
    SimpleDraw::Render(mCamera);
}

void SolarSystemState::DebugUI()
{
    ImGui::Begin("Solar System Debug");
    ImGui::Checkbox("Show Orbits", &mShowOrbits);
    ImGui::SliderFloat("Orbit Speed", &mGlobalOrbitSpeed, 0.0f, 5.0f);
    ImGui::SliderFloat("Rotation Speed", &mGlobalRotationSpeed, 0.0f, 5.0f);

    if (ImGui::BeginCombo("Select Planet", PlanetNames[mSelectedPlanet]))
    {
        for (int i = 0; i < static_cast<int>(PlanetNames.size()); ++i)
        {
            bool selected = (i == mSelectedPlanet);
            if (ImGui::Selectable(PlanetNames[i], selected))
                mSelectedPlanet = i;
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::Image(mRenderTarget.GetRawData(), { 128, 128 });
    ImGui::End();
}

void SolarSystemState::UpdateCamera(float deltaTime)
{
    auto* input = Input::InputSystem::Get();
    float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 2.0f;
    float turnSpeed = 0.1f;

    if (input->IsKeyDown(KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}