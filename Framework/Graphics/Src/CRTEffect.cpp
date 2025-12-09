#include "Precompiled.h"
#include "CRTEffect.h"

#include "RenderObject.h"
#include "VertexTypes.h"

#include <ImGui/Inc/imgui.h>

using namespace DgEngine;
using namespace DgEngine::Graphics;

void CRTEffect::Initialize(const std::filesystem::path& path)
{
    mSettingsBuffer.Initialize();

    // Screen quad should use VertexPX (POSITION + TEXCOORD)
    // If your screen quad uses a different vertex type, swap VertexPX for that.
    mVertexShader.Initialize<VertexPX>(path);
    mPixelShader.Initialize(path);

    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Clamp);
}

void CRTEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
    mSettingsBuffer.Terminate();

    mSourceTexture = nullptr;
}

void CRTEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();

    mSampler.BindPS(0);
    mSettingsBuffer.BindPS(0);
}

void CRTEffect::End()
{
    if (mSourceTexture != nullptr)
        Texture::UnbindPS(0);
}

void CRTEffect::Render(const RenderObject& renderObject)
{
    // Push settings to GPU
    mSettingsBuffer.Update(mSettingsData);

    if (mSourceTexture != nullptr)
        mSourceTexture->BindPS(0);

    renderObject.meshBuffer.Render();
}

void CRTEffect::SetSourceTexture(const Texture& texture)
{
    mSourceTexture = &texture;
}

void CRTEffect::SetTime(float time)
{
    mSettingsData.time = time;
}

void CRTEffect::SetDistortion(float value)
{
    mSettingsData.distortionStrength = value;
}

void CRTEffect::SetScanlineIntensity(float value)
{
    mSettingsData.scanlineIntensity = value;
}

void CRTEffect::SetNoiseIntensity(float value)
{
    mSettingsData.noiseIntensity = value;
}

void CRTEffect::SetVignetteStrength(float value)
{
    mSettingsData.vignetteStrength = value;
}

void CRTEffect::SetChromaOffset(float value)
{
    mSettingsData.chromaOffset = value;
}

void CRTEffect::SetBrightness(float value)
{
    mSettingsData.brightness = value;
}

void CRTEffect::SetContrast(float value)
{
    mSettingsData.contrast = value;
}

void CRTEffect::SetScreenSize(float width, float height)
{
    mSettingsData.screenSize = { width, height };
}

void CRTEffect::DebugUI()
{
    if (ImGui::CollapsingHeader("CRT Effect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Geometry / Distortion");
        ImGui::DragFloat("Distortion Strength", &mSettingsData.distortionStrength, 0.01f, 0.0f, 0.6f);
        ImGui::DragFloat("Vignette Strength", &mSettingsData.vignetteStrength, 0.01f, 0.0f, 3.0f);

        ImGui::Separator();
        ImGui::Text("Image");
        ImGui::DragFloat("Brightness", &mSettingsData.brightness, 0.01f, 0.5f, 2.0f);
        ImGui::DragFloat("Contrast", &mSettingsData.contrast, 0.01f, 0.5f, 2.0f);

        ImGui::Separator();
        ImGui::Text("Artifacts");
        ImGui::DragFloat("Scanline Intensity", &mSettingsData.scanlineIntensity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Noise Intensity", &mSettingsData.noiseIntensity, 0.01f, 0.0f, 0.5f);
        ImGui::DragFloat("Chroma Offset", &mSettingsData.chromaOffset, 0.01f, 0.0f, 2.0f);

        ImGui::Separator();
        ImGui::Text("Screen Size: %.0f x %.0f",
            mSettingsData.screenSize.x,
            mSettingsData.screenSize.y);
    }
}
