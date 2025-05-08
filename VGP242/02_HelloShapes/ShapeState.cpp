#include"ShapeState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Math;

void ShapeState::Initialize()
{
	mVertices.push_back({ { -0.5F,0.0F,0.0F } });
	mVertices.push_back({ {  0.0F,0.75F,0.0F } });
	mVertices.push_back({ {  0.5F,0.0F,0.0F } });

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
	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoSomething.fx";

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
