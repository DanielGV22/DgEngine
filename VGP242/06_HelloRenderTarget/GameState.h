#pragma once

#include"DgEngine/Inc/DgEngine.h"

class GameState : public DgEngine::AppState 
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	struct Object
	{
		DgEngine::Math::Matrix4 matWorld = DgEngine::Math::Matrix4::Identity;
		DgEngine::Graphics::MeshBuffer meshBuffer;
		DgEngine::Graphics::TextureId textureId = 0;
	};

	void UpdateCamera(float deltaTime);
	void RenderObject(const Object& object, const DgEngine::Graphics::Camera& camera);

	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::Camera mRenderTargetCamera;

	// GPU communication
	DgEngine::Graphics::ConstantBuffer mTransformBuffer;
	DgEngine::Graphics::VertexShader mVertexShader;
	DgEngine::Graphics::PixelShader mPixelShader;
	DgEngine::Graphics::Sampler mSampler;

	// render object
	Object mObject0;
	Object mObject1;

	// render target
	DgEngine::Graphics::RenderTarget mRenderTarget;

};
 


