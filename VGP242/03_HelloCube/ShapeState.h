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

	using Vertices = std::vector<DgEngine::Graphics::VertexPC>;
	Vertices mVertices;

	DgEngine::Graphics::MeshBuffer mMeshBuffer;
	DgEngine::Graphics::VertexShader mVertexShader;
	DgEngine::Graphics::PixelShader mPixelShader;

};

