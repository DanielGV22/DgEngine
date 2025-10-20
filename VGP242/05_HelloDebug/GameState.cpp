#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;


void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
}
void GameState::Terminate()
{
}
void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
}
void GameState::Render()
{
}

bool gInvertValue = false;
float gFloatVal = 0.0f;
Math::Vector3 gV0 = Math::Vector3::Zero;
Math::Vector3 gV1 = Math::Vector3::One;
Math::Vector3 gV2 = Math::Vector3::XAxis;
Color gColor = Colors::White;
Color gSavedColor = Colors::White;


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
        gSavedColor = gColor;
        gColor = Color(1.0f - gColor.r, 1.0f - gColor.g, 1.0f - gColor.b, gColor.a);
    }
    else
    {
        gColor = gSavedColor;
    }

    int currentShape = (int)gCurrentShape;
    if (ImGui::Combo("Shape", &currentShape, gShapeNames, std::size(gShapeNames)))
    {
        gCurrentShape = (Shape)currentShape;
    }

    ImGui::End();

    switch (gCurrentShape)
    {
    case Shape::None: break;
    case Shape::AABB:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddFilledAABB(gV0 + gV2, gV1 + gV2, gColor);
        break;
    }
    case Shape::AABBFilled:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddFilledAABB(gV0 + gV2, gV1 + gV2, gColor);
        break;
    }
    case Shape::Sphere:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddSphere(60, 60, gFloatVal, gColor, gV0 + gV2);
        break;
    }
    case Shape::GroundPlane:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddGroundPlane(gFloatVal, gColor);
        break;
    }
    case Shape::GroundCircle:
    {
        // ImGui::DragFloat("Min");
        // ImGui::DragFloat("Max");
        SimpleDraw::AddGroundCircle(60, gFloatVal, gColor, gV0 + gV2);
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

    SimpleDraw::Render(mCamera);
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