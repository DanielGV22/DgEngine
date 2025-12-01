#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"

namespace DgEngine::Graphics
{
	class RenderObject;
	class Texture;

	class PostProcessingEffect
	{
	  public:
		  enum class Mode
		  {
			  None,
			  Monochrome,
			  Invert,
			  Mirror,
			  Blur,
			  Combine2,
			  MotionBlur,
			  ChromaticAberration,
			  Wave
		  };

		void Initialize(const std::filesystem::path& filePath);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void SetTexture(const Texture* texture, uint32_t slot = 0);
		void SetMode(Mode mode);

		void DebuUI();
	  private:
		  struct PostProcessData
		  {
			  int mode = 0;
			  float param0 = 0.0f;
			  float param1 = 0.0f;
			  float param2 = 0.0f;
		  };

		  using PostProcessBuffer = TypedConstantBuffer<PostProcessData>;
		  PostProcessBuffer mPostProcessBuffer;

		  VertexShader mVertexShader;
		  PixelShader mPixelShader;
		  Sampler mSampler;
		  std::array<const Texture*, 4> mTextures;

		  Mode mMode = Mode::None;
	};
}