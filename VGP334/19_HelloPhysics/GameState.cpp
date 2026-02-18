#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;

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

	Mesh plane = MeshBuilder::CreatePlane(10, 10, 1.0f, true);
	mGroundObject.meshBuffer.Initialize(plane);
	mGroundObject.diffuseMapId = tm->LoadTexture("misc/concrete.jpg");
	mGroundShape.InitializeHull({ 5.0f, 0.5f, 5.0f }, { 0.0f, -0.5f, 0.0f });
	mGroundRigidBody.Initialize(mGroundObject.transform, mGroundShape);

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

	Mesh boxShape = MeshBuilder::CreateCube(1.0f);
	TextureId boxTexture = tm->LoadTexture("earth.jpg");

    float yOffset = 4.5f;
    float xOffset = 0.0f;
    int rowCount = 1;
    int boxIndex = 0;
    mBoxes.resize(10);
    while (boxIndex < mBoxes.size())
    {
        xOffset = -((static_cast<float>(rowCount) - 1.0f) * 0.5f);
        for (int r = 0; r < rowCount; r++)
        {
            BoxData& box = mBoxes[boxIndex];
            box.box.meshBuffer.Initialize(boxShape);
            box.box.diffuseMapId = boxTexture;
            box.box.transform.position.x = xOffset;
            box.box.transform.position.y = yOffset;
            box.box.transform.position.z = 4.0f;
            box.shape.InitializeBox({ 0.5f, 0.5f, 0.5f });
            xOffset += 1.0f;
            ++boxIndex;
        }
        yOffset -= 1.0f;
        rowCount += 1;
    }
    for (int i = mBoxes.size() - 1; i >= 0; --i)
    {
        mBoxes[i].rigidBody.Initialize(mBoxes[i].box.transform, mBoxes[i].shape, 4.0f);
    }

    int rows = 10;
    int cols = 10;
    mClothMesh = MeshBuilder::CreatePlane(rows, cols, 1.0f);
    for (Graphics::Vertex& v : mClothMesh.vertices)
    {
        v.position.y = 15.0f;
        v.position.z -= 5.0f;
    }
    uint32_t lastVertex = mClothMesh.vertices.size() - 1;
    uint32_t lastVertexOS = lastVertex - cols;
    mClothSoftBody.Initialize(mClothMesh, 1.0f, { lastVertex, lastVertexOS });
    mCloth.meshBuffer.Initialize(nullptr, sizeof(Vertex), mClothMesh.vertices.size(),
        mClothMesh.indices.data(), mClothMesh.indices.size());
    mCloth.diffuseMapId = tm->LoadTexture("venus.jpg");
}

void GameState::Terminate()
{
    mCloth.Terminate();
    mClothSoftBody.Terminate();
    for (BoxData& box : mBoxes)
    {
        box.rigidBody.Terminate();
        box.shape.Terminate();
        box.box.Terminate();
    }
    mStandardEffect.Terminate();

    mBallRigidBody.Terminate();
    mBallShape.Terminate();
    mBallObject.Terminate();

    mGroundRigidBody.Terminate();
    mGroundShape.Terminate();
    mGroundObject.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    if (InputSystem::Get()->IsMousePressed(MouseButton::LBUTTON))
    {
        Math::Vector3 spawnPos = mCamera.GetPosition() + (mCamera.GetDirection() * 0.5f);
        mBallRigidBody.SetPosition(spawnPos);
        mBallRigidBody.SetVelocity(mCamera.GetDirection() * 20.0f);
    }
}

void GameState::Render()
{
    mCloth.meshBuffer.Update(mClothMesh.vertices.data(), mClothMesh.vertices.size());
    mStandardEffect.Begin();
    mStandardEffect.Render(mBallObject);
    mStandardEffect.Render(mGroundObject);
    mStandardEffect.Render(mCloth);
    for (BoxData& box : mBoxes)
    {
        mStandardEffect.Render(box.box);
    }
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
	PhysicsWorld::Get()->DebugUI();

    ImGui::End();
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
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime); 
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}


