#pragma once
#include"DgEngine/Inc/DgEngine.h"

enum class ShapeType
{
    Cube,
    Pyramid,
    Rectangle
};

class ShapeState : public DgEngine::AppState
{
public:
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Render() override;

protected:
    void CreateShape();
    void SwitchShape(ShapeType type);

    DgEngine::Graphics::MeshPC mMesh;
    DgEngine::Graphics::Camera mCamera;
    DgEngine::Graphics::ConstantBuffer mTransformBuffer;
    DgEngine::Graphics::MeshBuffer mMeshBuffer;
    DgEngine::Graphics::VertexShader mVertexShader;
    DgEngine::Graphics::PixelShader mPixelShader;

    ShapeType mCurrentShape = ShapeType::Cube;
};
