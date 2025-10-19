#pragma once

#include"ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"

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

		void DebugUI();

	private:
		struct TransformData
		{
			Math::Matrix4 wvp; // world view projection 
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		TransformBuffer mTransformBuffer;
		VertexShader mVertexShader;
		PixelShader  mPixelShader;

		const Camera* mCamera = nullptr;
		


	};
}