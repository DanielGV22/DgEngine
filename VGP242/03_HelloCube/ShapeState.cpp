#include"ShapeState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Math;

void ShapeState::Initialize()
{
	// Create a shape
	CreateShape();
	mMeshBuffer.Initialize(mVertices.data(), sizeof(VertexPC), mVertices.size());

	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFilePath);
	mPixelShader.Initialize(shaderFilePath);
}

void ShapeState::Terminate()
{
	mVertices.clear();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void ShapeState::Update(float deltaTime)
{

}

void ShapeState::Render()
{
	// prepare gpu
	mVertexShader.Bind();
	mPixelShader.Bind();

	// draw
	mMeshBuffer.Render();
}

void ShapeState::CreateShape()
{
	 mVertices.push_back({ { -0.5F,0.0F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  0.0F,0.75F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.5F,0.0F,0.0F }, Colors::Goldenrod });
}

