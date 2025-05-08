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


	struct Vertex
	{
		DgEngine::Math::Vector3 position;
		DgEngine::Graphics::Color color;
	};
	using Vertices = std::vector<Vertex>;
	Vertices mVertices;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;

};

class TriangleShapeState : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};

class SquareShapeState : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};

class HouseShapeState : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};

class RombusShapeState : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};