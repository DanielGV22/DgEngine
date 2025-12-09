#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "Texture.h"

namespace DgEngine::Graphics
{
    class RenderObject;

    class CRTEffect
    {
    public:
        void Initialize(const std::filesystem::path& path);
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);

        void SetSourceTexture(const Texture& texture);

        // Optional external setters
        void SetTime(float time);
        void SetDistortion(float value);
        void SetScanlineIntensity(float value);
        void SetNoiseIntensity(float value);
        void SetVignetteStrength(float value);
        void SetChromaOffset(float value);
        void SetBrightness(float value);
        void SetContrast(float value);
        void SetScreenSize(float width, float height);

        void DebugUI();

    private:
        struct SettingsData
        {
            // row 0
            float distortionStrength = 0.2f;   // barrel distortion
            float scanlineIntensity = 0.5f;   // 0..1
            float noiseIntensity = 0.08f;  // 0..1
            float vignetteStrength = 1.0f;   // 0..3

            // row 1
            float time = 0.0f;           // seconds
            float chromaOffset = 1.0f;         // chromatic aberration scale
            float brightness = 1.0f;           // post brightness
            float contrast = 1.0f;           // post contrast

            // row 2
            Math::Vector2 screenSize = { 1280.0f, 720.0f };
            Math::Vector2 padding = { 0.0f, 0.0f }; // for alignment
        };

        using SettingsBuffer = TypedConstantBuffer<SettingsData>;
        SettingsBuffer mSettingsBuffer;

        VertexShader mVertexShader;
        PixelShader  mPixelShader;
        Sampler      mSampler;

        const Texture* mSourceTexture = nullptr;

        SettingsData mSettingsData;
    };
}
