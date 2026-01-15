#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;

void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.0f, -6.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

    // Light setup
    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 1.0f, 1.0f, 1.0f, 1.0f };

    auto tm = TextureManager::Get();

    //  Earth
    {
        Mesh mesh = MeshBuilder::CreateSphere(30, 30, 0.30f);
        mEarth.meshBuffer.Initialize(mesh);
        mEarth.diffuseMapId = tm->LoadTexture("earth.jpg");
        mEarth.transform.position = { -1.5f, 0.0f, 0.0f }; // move slightly left
    }

    //  Mars
    {
        Mesh mesh = MeshBuilder::CreateSphere(30, 30, 0.6f);
        mMars.meshBuffer.Initialize(mesh);
        mMars.diffuseMapId = tm->LoadTexture("Mars.png");
        mMars.transform.position = { 2.5f, 0.0f, 0.0f }; 
    }

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

	mAnimationTime = 0.0f;
    mAnimation = AnimationBuilder()
        .AddPositionKey({ 0.0f, 0.0f, 0.0f }, 0.0f)
        .AddPositionKey({ 1.0f, 1.0f, 2.0f }, 3.0f)
        .AddPositionKey({ 0.0f, 0.0f, 0.0f }, 5.0f)
        .AddRotationKey(Math::Quaternion::Identity, 0.0f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 90.0f * Math::Constants::DegToRad), 2.0f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 180.0f * Math::Constants::DegToRad), 3.1f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 270.0f * Math::Constants::DegToRad), 4.0f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 360.0f * Math::Constants::DegToRad), 5.0f)
		.AddScaleKey(Math::Vector3::One, 0.0f)
		.AddScaleKey(Math::Vector3::One * 0.0f, 3.0f)
		.AddScaleKey(Math::Vector3::One, 5.0f)
        .Build();
}

void GameState::Terminate()
{
    mEarth.Terminate();
    mMars.Terminate();
    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

	mAnimationTime += deltaTime;
	while (mAnimationTime > mAnimation.GetDuration())
    {
        mAnimationTime -= mAnimation.GetDuration();
    }
}

void GameState::Render()
{
	mEarth.transform = mAnimation.GetTransform(mAnimationTime);

    SimpleDraw::AddGroundPlane(10.0f, Colors::DarkGray);
    SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();
    mStandardEffect.Render(mEarth);
    mStandardEffect.Render(mMars);
    mStandardEffect.End();
}



void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if(ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
        {
            mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

        ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
        ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
    }
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
      
        ImGui::ColorEdit4("Emmisive#Material", &mEarth.material.emissive.r);
        ImGui::ColorEdit4("Ambient#Material", &mEarth.material.ambient.r);
        ImGui::ColorEdit4("Diffuse#Material", &mEarth.material.diffuse.r);
        ImGui::ColorEdit4("Specular#Material", &mEarth.material.specular.r);
        ImGui::DragFloat("Shininess#Material", &mEarth.material.shininess, 0.01f, 0.0f, 10000.0f);
    }

	mStandardEffect.DebugUI();
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
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime); 
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}


