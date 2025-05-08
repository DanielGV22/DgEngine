#include"ShapeState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Math;

void ShapeState::Initialize()
{
	// Create a shape
	CreateShape();

	auto device = GraphicsSystem::Get()->GetDevice();

	// need buffer to store vertces
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth=static_cast<UINT>(sizeof(Vertex) * mVertices.size());
	bufferDesc.Usage=D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.MiscFlags=0;
	bufferDesc.StructureByteStride=0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = mVertices.data();

	HRESULT hr= device->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
	ASSERT(SUCCEEDED(hr), "Failed to create vertex buffer");
	//===============================================================================

	// bind to function in specified shader file 
	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoColor.fx";

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | 		D3DCOMPILE_DEBUG;
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3DCompileFromFile(
		shaderFilePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		shaderFlags, 0,
		&shaderBlob,
		&errorBlob);
	if (errorBlob!= nullptr && errorBlob->GetBufferPointer()!=nullptr)
	{
		LOG("%s",static_cast<const char*>(errorBlob->GetBufferPointer()));
	}
	ASSERT(SUCCEEDED(hr), "Failed to compile vertex shader");

	hr=device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&mVertexShader);
	ASSERT(SUCCEEDED(hr), "Failed to create vertex shader");
	//==========================================================================

	// STATE WHAT VERTEX VARIABLES ARE
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexLayout;
	vertexLayout.push_back({ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT });
	vertexLayout.push_back({ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT });

	hr=device->CreateInputLayout(
		vertexLayout.data(),
		static_cast<UINT>(vertexLayout.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		&mInputLayout);
	ASSERT(SUCCEEDED(hr), "Failed to create input layout");
	SafeRelease(shaderBlob);
	SafeRelease(errorBlob);
	//==========================================================================

	// BIND TO PIXEL FUNCTION IN SPECIFIED SHADER FILE
	hr=D3DCompileFromFile(
		shaderFilePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		shaderFlags, 0,
		&shaderBlob,
		&errorBlob);
	if (errorBlob != nullptr && errorBlob->GetBufferPointer() != nullptr)
	{
		LOG("%s", static_cast<const char*>(errorBlob->GetBufferPointer()));
	}
	ASSERT(SUCCEEDED(hr), "Failed to compile pixel shader");

	hr = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&mPixelShader);
	ASSERT(SUCCEEDED(hr), "Failed to create pixel shader");
	SafeRelease(shaderBlob);
	SafeRelease(errorBlob);
}

void ShapeState::Terminate()
{
	mVertices.clear();
	SafeRelease(mPixelShader);
	SafeRelease(mInputLayout);
	SafeRelease(mVertexShader);
	SafeRelease(mVertexBuffer);
}

void ShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		DgEngine::MainApp().ChangeState("TriangleShapeState");
	}
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		DgEngine::MainApp().ChangeState("SquareShapeState");
	}
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		DgEngine::MainApp().ChangeState("HouseShapeState");
	}
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		DgEngine::MainApp().ChangeState("RombusShapeState");
	}
}

void ShapeState::Render()
{
	auto context= GraphicsSystem::Get()->GetContext();
	// bind buffers
	context->VSSetShader(mVertexShader, nullptr, 0);
	context->IASetInputLayout(mInputLayout);
	context->PSSetShader(mPixelShader, nullptr, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Draw
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->Draw(static_cast<UINT>(mVertices.size()), 0);
}

void ShapeState::CreateShape()
{
	 mVertices.push_back({ { -0.5F,0.0F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  0.0F,0.75F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.5F,0.0F,0.0F }, Colors::Goldenrod });
}

void TriangleShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		DgEngine::MainApp().ChangeState("ShapeState");
	}
}

void TriangleShapeState::CreateShape()
{
	 mVertices.push_back({ { -0.75F,-0.75F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  -0.5F,0.0F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  -0.25F,-0.75F,0.0F }, Colors::Goldenrod });

	 mVertices.push_back({ { -0.5F,0.0F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ { 0.0F,0.75F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.5F,0.0F,0.0F }, Colors::Goldenrod });

	 mVertices.push_back({ { 0.25F,-0.75F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  0.5F,0.0F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.75F,-0.75F,0.0F }, Colors::Goldenrod });

}

void SquareShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		DgEngine::MainApp().ChangeState("ShapeState");
	}
}

void SquareShapeState::CreateShape()
{
		 mVertices.push_back({ { -0.5F,-0.5F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  -0.5F,0.5F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.5F,0.5F,0.0F }, Colors::Goldenrod });

	 mVertices.push_back({ { -0.5F,-0.5F,0.0F }, Colors::AliceBlue });
	 mVertices.push_back({ {  0.5F,0.5F,0.0F }, Colors::Crimson });
	 mVertices.push_back({ {  0.5F,-0.5F,0.0F }, Colors::Goldenrod });
}

void HouseShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		DgEngine::MainApp().ChangeState("ShapeState");
	}
}

void HouseShapeState::CreateShape()
{
	
	mVertices.push_back({ { -0.5f,  0.5f, 0.0f }, Colors::Brown });  
	mVertices.push_back({ {  0.5f,  0.5f, 0.0f }, Colors::Brown });  
	mVertices.push_back({ { -0.5f, -0.5f, 0.0f }, Colors::Brown });  

	mVertices.push_back({ { -0.5f, -0.5f, 0.0f }, Colors::Brown });  
	mVertices.push_back({ {  0.5f,  0.5f, 0.0f }, Colors::Brown });  
	mVertices.push_back({ {  0.5f, -0.5f, 0.0f }, Colors::Brown });  
	
	mVertices.push_back({ { -0.5f,  0.5f, 0.0f }, Colors::Green});   
	mVertices.push_back({ {  0.0f,  1.0f, 0.0f }, Colors::Green});   
	mVertices.push_back({ {  0.5f,  0.5f, 0.0f }, Colors::Green});   
}

void RombusShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		DgEngine::MainApp().ChangeState("ShapeState");
	}
}

void RombusShapeState::CreateShape()
{
	
	mVertices.push_back({ { -0.5f, 0.0f, 0.0f }, Colors::AliceBlue });  
	mVertices.push_back({ { 0.0f, 0.5f, 0.0f }, Colors::Crimson });   
	mVertices.push_back({ { 0.5f, 0.0f, 0.0f }, Colors::Goldenrod }); 
	mVertices.push_back({ { 0.0f, -0.5f, 0.0f }, Colors::AliceBlue }); 
	mVertices.push_back({ { -0.5f, 0.0f, 0.0f }, Colors::Crimson }); 
	mVertices.push_back({ { 0.5f, 0.0f, 0.0f }, Colors::Goldenrod }); 
}
