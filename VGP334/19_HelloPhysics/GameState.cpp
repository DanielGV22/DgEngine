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

	//  Ball Object
    {
        Mesh mesh = MeshBuilder::CreateSphere(30, 30, 0.30f);
        mBallObject.meshBuffer.Initialize(mesh);
        mBallObject.diffuseMapId = tm->LoadTexture("misc/basketball.jpg");
		mBallObject.transform.position.y = 0.5f;
		mBallShape.InitializeSphere(0.25f);
		mBallRigidBody.Initialize(mBallObject.transform, mBallShape, 1.0f);
    }

	Mesh plane = MeshBuilder::CreatePlane(10.0f, 10.0f, 1.0f,true);
	mGroundObject.meshBuffer.Initialize(plane);
	mGroundObject.diffuseMapId = tm->LoadTexture("misc/concrete.jpg");
	mGroundShape.InitializeHull({ 5.0f, 0.5f, 5.0f }, { 0.0f, -0.5f, 0.0f });
	mGroundRigidBody.Initialize(mGroundObject.transform, mGroundShape);

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

}

void GameState::Terminate()
{
    mStandardEffect.Terminate();
	mGroundRigidBody.Terminate();
	mGroundShape.Terminate();
	mGroundObject.Terminate();
    mBallRigidBody.Terminate();
	mBallShape.Terminate();
    mBallObject.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

}

void GameState::Render()
{
    mStandardEffect.Begin();
       mStandardEffect.Render(mBallObject);
	   mStandardEffect.Render(mGroundObject);
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
   
	Math::Vector3 pos = mBallObject.transform.position;
    if (ImGui::DragFloat3("BallPosition", &pos.x))
    {
		mBallObject.transform.position = pos;
        mBallRigidBody.SetPosition(mBallObject.transform.position);
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


