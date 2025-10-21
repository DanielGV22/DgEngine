 #include "Precompiled.h"
#include "StandardEffect.h"

#include "VertexTypes.h"
#include "Camera.h"
#include "RenderObject.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

void StandardEffect::Initialize(const std::filesystem::path& path)
{
	//buffers
	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	//other stuff
	mVertexShader.Initialize<Vertex>(path);
	mPixelShader.Initialize(path);
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void StandardEffect::Terminate()
{
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();
}
	
void StandardEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mSampler.BindVS(0);
	mSampler.BindPS(0);

	mTransformBuffer.BindVS(0);
	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);
	mMaterialBuffer.BindPS(2);
	mSettingsBuffer.BindVS(3);
	mSettingsBuffer.BindPS(3);

}
void StandardEffect::End()
{

}
	
void StandardEffect::Render(const RenderObject& renderObject)
{
	const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
	const Math::Matrix4 matView = mCamera->GetViewMatrix();
	const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
	const Math::Matrix4 matFinal = matWorld * matView * matProj;

	TransformData data;
	data.wvp = Math::Transpose(matFinal);
	data.world = Math::Transpose(matWorld);
	data.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(data);

	SettingsData settings;
	settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
	settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
	settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
	settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
	settings.bumpWeight = mSettingsData.bumpWeight;
	mSettingsBuffer.Update(settings);

	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update(renderObject.material);

	TextureManager* tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specMapId, 1);
	tm->BindPS(renderObject.normalMapId, 2);
	tm->BindVS(renderObject.bumpMapId, 3);

	renderObject.meshBuffer.Render();
}
	
void StandardEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}
	
void StandardEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void StandardEffect::DebugUI() 
{
	if (ImGui::CollapsingHeader("StandardEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool useDiffuseMap = mSettingsData.useDiffuseMap > 0;
		if(ImGui::Checkbox("Use Diffuse Map", &useDiffuseMap))
		{
			mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;
		}
		bool useSpecMap = mSettingsData.useSpecMap > 0;
		if(ImGui::Checkbox("Use Specular Map", &useSpecMap))
		{
			mSettingsData.useSpecMap = useSpecMap ? 1 : 0;
		}
		bool useNormalMap = mSettingsData.useNormalMap > 0;
		if(ImGui::Checkbox("Use Normal Map", &useNormalMap))
		{
			mSettingsData.useNormalMap = useNormalMap ? 1 : 0;
		}
		bool useBumpMap = mSettingsData.useBumpMap > 0;
		if(ImGui::Checkbox("Use Bump Map", &useBumpMap))
		{
			mSettingsData.useBumpMap = useBumpMap ? 1 : 0;
		}
		ImGui::DragFloat("Bump Weight", &mSettingsData.bumpWeight, 0.01f, 0.0f, 100.0f);
	}
}