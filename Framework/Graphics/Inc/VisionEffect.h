#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "Texture.h"

namespace DgEngine::Graphics
{
    class RenderObject;

    class VisionEffect
    {
    public:
        void Initialize(const std::filesystem::path& path);
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);

        void SetSourceTexture(const Texture& texture);

        void SetMode(int mode);            // 0 = off, 1 = thermal, 2 = night vision
        void SetTime(float time);
        void SetNoiseIntensity(float value);
        void SetVignetteStrength(float value);
        void SetThermalIntensity(float value);
        void SetNightIntensity(float value);
        void SetScreenSize(float width, float height);

        void DebugUI();

    private:
        struct SettingsData
        {
            float mode = 0.0f;
            float time = 0.0f;
            float noiseIntensity = 0.15f;
            float vignetteStrength = 1.0f;
            float thermalIntensity = 1.2f;
            float nightIntensity = 1.2f;
            Math::Vector2 screenSize = { 1280.0f, 720.0f };
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
