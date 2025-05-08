#pragma once
#include"DgEngine/Inc/DgEngine.h"

class ShapeState : public DgEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render()override;

private:
	struct Vertex
	{
		DgEngine::Math::Vector3 position;
	};
	using Vertices = std::vector<Vertex>;
	Vertices mVertices;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;

};