#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;


enum class CameraTarget
{
    Sun,
    Earth
};

CameraTarget gCurrentTarget = CameraTarget::Sun;
const char* gTargetNames[] = { "Sun", "Earth" };


void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

    mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
    mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
    mRenderTargetCamera.SetAspectRatio(1.0f);

    // initialize gpu communication
    std::filesystem::path shaderFile = L"../../Assets/Shaders/DoTexture.fx";
    mVertexShader.Initialize<VertexPX>(shaderFile);
    mPixelShader.Initialize(shaderFile);
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
    mTransformBuffer.Initialize(sizeof(Math::Matrix4));

    // initialize render object
    MeshPX sphere = MeshBuilder::CreateSpherePX(60, 60, 1.0f);
    mObject0.meshBuffer.Initialize(sphere);
    mObject1.meshBuffer.Initialize(sphere);


    mObject0.textureId = TextureManager::Get()->LoadTexture(L"sun.jpg");
    mObject1.textureId = TextureManager::Get()->LoadTexture(L"earth.jpg");
    mObject1.matWorld = Math::Matrix4::Translation({ 0.0f, 1.0f, 0.0f });

    constexpr uint32_t size = 512;
    mRenderTarget.Initialize(size, size, RenderTarget::Format::RGBA_U32);
}
void GameState::Terminate()
{
    mRenderTarget.Terminate();
    TextureManager::Get()->ReleaseTexture(mObject0.textureId);
    TextureManager::Get()->ReleaseTexture(mObject1.textureId);
    mObject0.meshBuffer.Terminate();
    mObject1.meshBuffer.Terminate();
    mTransformBuffer.Terminate();
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
}
void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

    Math::Vector3 targetPosition = Math::Vector3::Zero;
    switch (gCurrentTarget)
    {
    case CameraTarget::Sun:
        targetPosition = Math::GetTranslation(mObject0.matWorld);
        break;
    case CameraTarget::Earth:
        targetPosition = Math::GetTranslation(mObject1.matWorld);
        break;
    }

    mRenderTargetCamera.SetLookAt(targetPosition);
}
void GameState::Render()
{
    SimpleDraw::AddGroundPlane(10.0f, Colors::DarkGray);
    SimpleDraw::Render(mCamera);

    // render to the render target
    mRenderTarget.BeginRender();
    switch (gCurrentTarget)
    {
    case CameraTarget::Sun:
        RenderObject(mObject0, mRenderTargetCamera);
        break;
    case CameraTarget::Earth:
        RenderObject(mObject1, mRenderTargetCamera);
        break;
    }
    mRenderTarget.EndRender();

    // render to the scene
    RenderObject(mObject0, mCamera);
    RenderObject(mObject1, mCamera);
}

void GameState::RenderObject(const Object& object, const Camera& camera)
{
    const Math::Matrix4 matView = camera.GetViewMatrix();
    const Math::Matrix4 matProj = camera.GetProjectionMatrix();
    const Math::Matrix4 matFinal = object.matWorld * matView * matProj;
    const Math::Matrix4 wvp = Math::Transpose(matFinal);
    mTransformBuffer.Update(&wvp);

    mVertexShader.Bind();
    mPixelShader.Bind();
    mSampler.BindPS(0);
    mTransformBuffer.BindVS(0);

    TextureManager::Get()->BindPS(object.textureId, 0);
    object.meshBuffer.Render();
}

bool gInvertValue = false;
float gFloatVal = 0.0f;
Math::Vector3 gV0 = Math::Vector3::Zero;
Math::Vector3 gV1 = Math::Vector3::One;
Math::Vector3 gV2 = Math::Vector3::XAxis;
Color gDisplayColor = Colors::White;
Color gColor = Colors::White;

enum class Shape
{
    None,
    AABB,
    AABBFilled,
    Sphere,
    GroundPlane,
    GroundCircle,
    Transform
};
const char* gShapeNames[] =
{
    "None",
    "AABB",
    "AABBFilled",
    "Sphere",
    "GroundPlane",
    "GroundCircle",
    "Transform"
};

Shape gCurrentShape = Shape::None;
void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Hello Yall");
    ImGui::Checkbox("Invert Color", &gInvertValue);
    ImGui::DragFloat("Float", &gFloatVal);
    ImGui::DragFloat3("V0", &gV0.x, 0.1f);
    ImGui::DragFloat3("V1", &gV1.x, 0.1f);
    ImGui::DragFloat3("V2", &gV2.x, 0.1f);
    ImGui::ColorEdit4("Color", &gColor.r);

    if (gInvertValue)
    {
        gDisplayColor = Color(1.0f - gColor.r, 1.0f - gColor.g, 1.0f - gColor.b, gColor.a);
    }
    else
    {
        gDisplayColor = gColor;
    }

    int currentShape = (int)gCurrentShape;
    if (ImGui::Combo("Shape", &currentShape, gShapeNames, std::size(gShapeNames)))
    {
        gCurrentShape = (Shape)currentShape;
    }


    switch (gCurrentShape)
    {
    case Shape::None: break;
    case Shape::AABB:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddAABB(gV0 + gV2, gV1 + gV2, gDisplayColor);
        break;
    }
    case Shape::AABBFilled:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddFilledAABB(gV0 + gV2, gV1 + gV2, gDisplayColor);
        break;
    }
    case Shape::Sphere:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddSphere(60, 60, gFloatVal, gDisplayColor, gV0 + gV2);
        break;
    }
    case Shape::GroundPlane:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddGroundPlane(gFloatVal, gDisplayColor);
        break;
    }
    case Shape::GroundCircle:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddGroundCircle(60, gFloatVal, gDisplayColor, gV0 + gV2);
        break;
    }
    case Shape::Transform:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddTransform(Math::Matrix4::Translation(gV2));
        break;
    }
    }

    ImGui::Separator();
    ImGui::Text("RenderTarget");
    ImGui::Image(
        mRenderTarget.GetRawData(),
        { 128, 128 },
        { 0, 0 },
        { 1, 1 },
        { 1, 1, 1, 1 },
        { 1, 1, 1, 1 }
    );

    int currentTarget = static_cast<int>(gCurrentTarget);
    if (ImGui::Combo("Camera Target", &currentTarget, gTargetNames, IM_ARRAYSIZE(gTargetNames)))
    {
        gCurrentTarget = static_cast<CameraTarget>(currentTarget);

        Math::Vector3 targetPosition = Math::Vector3::Zero;
        switch (gCurrentTarget)
        {
        case CameraTarget::Sun:
            targetPosition = Math::GetTranslation(mObject0.matWorld);
            break;
        case CameraTarget::Earth:
            targetPosition = Math::GetTranslation(mObject1.matWorld);
            break;
        }

        mRenderTargetCamera.SetLookAt(targetPosition);
    }


    ImGui::End();
}


void GameState::UpdateCamera(float deltaTime)
{
    Input::InputSystem* input = Input::InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
    const float turnSpeed = 0.1f;

    if (input->IsKeyDown(KeyCode::W))
    {
        mCamera.Walk(moveSpeed * deltaTime);
    }
    if (input->IsKeyDown(KeyCode::S))
    {
        mCamera.Walk(-moveSpeed * deltaTime);
    }
    if (input->IsKeyDown(KeyCode::D))
    {
        mCamera.Strafe(moveSpeed * deltaTime);
    }
    if (input->IsKeyDown(KeyCode::A))
    {
        mCamera.Strafe(-moveSpeed * deltaTime);
    }
    if (input->IsKeyDown(KeyCode::E))
    {
        mCamera.Rise(moveSpeed * deltaTime);
    }
    if (input->IsKeyDown(KeyCode::Q))
    {
        mCamera.Rise(-moveSpeed * deltaTime);
    }

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime); // INVERT IF U WANT
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}