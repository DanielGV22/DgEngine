#pragma once

#include"ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Sampler.h"

namespace DgEngine::Graphics
{
	class Camera;
	class RenderObject;

	class StandardEffect final
	{
	public:
		void Initialize(const std::filesystem::path& path);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);

		void DebugUI();

	private:
		struct TransformData
		{
			Math::Matrix4 wvp;   // world view projection 
			Math::Matrix4 world; // world matrix
			Math::Vector3 viewPosition; // position of the view item (camera)
			float padding = 0.0f; // padding to maintain 16 byte alignment
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useSpecMap = 1;
			int useNormalMap = 1;
			int useBumpMap = 1;
			float bumpWeight = 0.1f;
			float padding[3] = { 0.0f }; // padding to maintain 16 byte alignment
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		TransformBuffer mTransformBuffer;

		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		LightBuffer mLightBuffer;

		using MaterialBuffer = TypedConstantBuffer<Material>;
		MaterialBuffer mMaterialBuffer;

		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		SettingsBuffer mSettingsBuffer;

		VertexShader mVertexShader;
		PixelShader  mPixelShader;
		Sampler      mSampler;

		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;
		SettingsData mSettingsData;
		


	};
}