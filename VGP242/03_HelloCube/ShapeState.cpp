#include"ShapeState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Math;
using namespace DgEngine::Input;

void ShapeState::Initialize()
{
	mCamera.SetPosition({ 0.0f,1.0f,-3.0f });
	mCamera.SetLookAt({ 0.0f,0.0f,0.0f });

	mTransformBuffer.Initialize(sizeof(Math::Matrix4));

	// Create a shape
	CreateShape();
	mMeshBuffer.Initialize(mMesh);

	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/Do TransformColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFilePath);
	mPixelShader.Initialize(shaderFilePath);
}

void ShapeState::Terminate()
{

	mTransformBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void ShapeState::Update(float deltaTime)
{
    InputSystem* input= InputSystem::Get();
	const float moveSpeed= input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
	const float turnSpeed= 1.0f;

	if (input->IsKeyDown(KeyCode::W))
	{
		mCamera.Walk(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::S))
	{
		mCamera.Walk(-moveSpeed * deltaTime);
	}
	if (input->IsKeyDown(KeyCode::A))
	{
		mCamera.Strafe(-moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::D))
	{
		mCamera.Strafe(moveSpeed * deltaTime);
	}
	if (input->IsKeyDown(KeyCode::Q))
	{
		mCamera.Rise(-moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::E))
	{
		mCamera.Rise(moveSpeed * deltaTime);
	}

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}

}

void ShapeState::Render()
{
	// prepare gpu
	mVertexShader.Bind();
	mPixelShader.Bind();

	// sync transform buffer
	mTransformBuffer.BindVS(0);

	//update the buffer data
	Math::Matrix4 matWorld = Math::Matrix4::Identity;
	Math::Matrix4 matView = mCamera.GetViewMatrix();
	Math::Matrix4 matProjection = mCamera.GetProjectionMatrix();
	Math::Matrix4 matFinal = matWorld * matView * matProjection;
	Math::Matrix4 wvp = Math::Transpose(matFinal);
	mTransformBuffer.Update(&wvp);

	// draw
	mMeshBuffer.Render();
}

void ShapeState::CreateShape()
{
	//mMesh = MeshBuilder::CreateCubePC(1.0f);
	//mMesh=MeshBuilder::CreatePyramidPC(1.0f);
	//mMesh=MeshBuilder::CreateRectanglePC(1.0f, 1.0f,1.0f);
	//mMesh = MeshBuilder::CreatePlanePC(10, 10, 1.0f, true);
	//mMesh = MeshBuilder::CreateCylinderPC(30, 3);
	mMesh = MeshBuilder::CreateSpherePC(30, 30, 1.0f);
}

