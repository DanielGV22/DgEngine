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
	class RenderGroup;
	class Texture;

	class StandardEffect final
	{
	public:
		void Initialize(const std::filesystem::path& path);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);
		void Render(const RenderGroup& renderGroup);

		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetLightCamera(const Camera& camera);
		void SetShadowMap(const Texture& shadowMap);

		void DebugUI();

	private:
		struct TransformData
		{
			Math::Matrix4 wvp; // world view projection
			Math::Matrix4 world; // world matrix
			Math::Matrix4 lwvp; // world view projection of the light object for shadows
			Math::Vector3 viewPosition; // camera position
			float padding = 0.0f; // padding to maintain 16 byte alignment
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useSpecMap = 1;
			int useNormalMap = 1;
			int useBumpMap = 1;
			int useShadowMap = 1;
			int useSkinning = 1;
			float bumpWeight = 0.1f;
			float depthBias = 0.000003f;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		TransformBuffer mTransformBuffer;

		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		LightBuffer mLightBuffer;

		using MaterialBurffer = TypedConstantBuffer<Material>;
		MaterialBurffer mMaterialBuffer;

		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		SettingsBuffer mSettingsBuffer;

		using BoneTransformBuffer = ConstantBuffer;
		BoneTransformBuffer mBoneTransformBuffer;

		VertexShader mVertexShader;
		PixelShader mPixelShader;
		Sampler mSampler;

		SettingsData mSettingsData;
		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;
		const Camera* mLightCamera = nullptr;
		const Texture* mShadowMap = nullptr;
	};
}