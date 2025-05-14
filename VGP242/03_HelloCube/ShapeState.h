#pragma once
#include"DgEngine/Inc/DgEngine.h"

class ShapeState : public DgEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render()override;

protected:
	virtual void CreateShape();

	DgEngine::Graphics::MeshPC mMesh;

	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::ConstantBuffer mTransformBuffer;
	DgEngine::Graphics::MeshBuffer mMeshBuffer;
	DgEngine::Graphics::VertexShader mVertexShader;
	DgEngine::Graphics::PixelShader mPixelShader;

};

