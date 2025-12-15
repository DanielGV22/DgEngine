#include "Precompiled.h"
#include "VisionEffect.h"

#include "VertexTypes.h"
#include "RenderObject.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

void VisionEffect::Initialize(const std::filesystem::path& path)
{
    mSettingsBuffer.Initialize();

    // Screen quad uses VertexPX (position + texcoord)
    mVertexShader.Initialize<VertexPX>(path);
    mPixelShader.Initialize(path);

    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Clamp);
}

void VisionEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
    mSettingsBuffer.Terminate();
}

void VisionEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();

    mSampler.BindPS(0);

    mSettingsBuffer.BindPS(0);
}

void VisionEffect::End()
{
    if (mSourceTexture != nullptr)
    {
        Texture::UnbindPS(0);
    }
}

void VisionEffect::Render(const RenderObject& renderObject)
{
    // Update settings buffer
    mSettingsBuffer.Update(mSettingsData);

    if (mSourceTexture != nullptr)
    {
        mSourceTexture->BindPS(0);
    }

    renderObject.meshBuffer.Render();
}

void VisionEffect::SetSourceTexture(const Texture& texture)
{
    mSourceTexture = &texture;
}

void VisionEffect::SetMode(int mode)
{
    mSettingsData.mode = static_cast<float>(mode);
}

void VisionEffect::SetTime(float time)
{
    mSettingsData.time = time;
}

void VisionEffect::SetNoiseIntensity(float value)
{
    mSettingsData.noiseIntensity = value;
}

void VisionEffect::SetVignetteStrength(float value)
{
    mSettingsData.vignetteStrength = value;
}

void VisionEffect::SetThermalIntensity(float value)
{
    mSettingsData.thermalIntensity = value;
}

void VisionEffect::SetNightIntensity(float value)
{
    mSettingsData.nightIntensity = value;
}

void VisionEffect::SetScreenSize(float width, float height)
{
    mSettingsData.screenSize = { width, height };
}

void VisionEffect::DebugUI()
{
    if (ImGui::CollapsingHeader("Vision Effect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // --- Mode selector (0 = Off, 1 = Thermal, 2 = Night Vision) ---
        const char* modes[] = { "Off", "Thermal", "Night Vision" };
        int modeInt = static_cast<int>(mSettingsData.mode);   // from float
        if (ImGui::Combo("Mode", &modeInt, modes, IM_ARRAYSIZE(modes)))
        {
            mSettingsData.mode = static_cast<float>(modeInt);  // write back
        }

        ImGui::Separator();
        ImGui::Text("General");
        ImGui::DragFloat("Noise Intensity", &mSettingsData.noiseIntensity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Vignette Strength", &mSettingsData.vignetteStrength, 0.01f, 0.0f, 3.0f);

        ImGui::Separator();
        ImGui::Text("Thermal");
        ImGui::DragFloat("Thermal Intensity", &mSettingsData.thermalIntensity, 0.01f, 0.1f, 3.0f);

        ImGui::Separator();
        ImGui::Text("Night Vision");
        ImGui::DragFloat("Night Intensity", &mSettingsData.nightIntensity, 0.01f, 0.1f, 3.0f);

        ImGui::Separator();
        ImGui::Text("Screen Size: %.0f x %.0f",
            mSettingsData.screenSize.x,
            mSettingsData.screenSize.y);
    }
}
