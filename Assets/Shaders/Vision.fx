// Vision.fx
// Post-process thermal / night vision shader for screen quad (VertexPX)

Texture2D gSceneColor : register(t0);
SamplerState gSampler : register(s0);

cbuffer PostProcessBuffer : register(b0)
{
    float mode; // 0 = off, 1 = thermal, 2 = night vision
    float time; // for animated noise
    float noiseIntensity; // 0–1
    float vignetteStrength; // 0–2
    float thermalIntensity; // 0–2
    float nightIntensity; // 0–2
    float2 screenSize; // width, height
};

// Must match VertexPX: POSITION + TEXCOORD
struct VS_INPUT
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    // Screen quad is already in clip space (-1..1)
    output.position = float4(input.position, 1.0f);
    output.texcoord = input.texcoord;
    return output;
}

// Simple hash-based noise
float Hash(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453);
}

float3 ApplyVignette(float3 color, float2 uv)
{
    float2 centered = uv * 2.0f - 1.0f; // [-1,1]
    float dist = length(centered);
    float vig = saturate(1.0f - dist * vignetteStrength);
    return color * vig;
}

float3 ApplyNoise(float3 color, float2 uv)
{
    if (noiseIntensity <= 0.0f)
        return color;

    float2 pixelCoord = uv * screenSize;
    float n = Hash(pixelCoord + time * 60.0f);
    float3 noise = (n - 0.5f).xxx; // centered noise
    return lerp(color, color + noise, noiseIntensity);
}

// Thermal mapping using luminance -> gradient
float3 ApplyThermal(float3 color)
{
    // Get brightness as "heat"
    float gray = dot(color, float3(0.299f, 0.587f, 0.114f));

    // Optional intensity
    gray = saturate(gray * thermalIntensity);

    // Color ramp: blue -> cyan -> green -> yellow -> red
    float3 cCold = float3(0.0, 0.0, 0.2);
    float3 cMid1 = float3(0.0, 0.8, 1.0);
    float3 cMid2 = float3(0.0, 1.0, 0.0);
    float3 cHot1 = float3(1.0, 1.0, 0.0);
    float3 cHot2 = float3(1.0, 0.0, 0.0);

    float3 result;
    if (gray < 0.25f)
    {
        float t = gray / 0.25f;
        result = lerp(cCold, cMid1, t);
    }
    else if (gray < 0.5f)
    {
        float t = (gray - 0.25f) / 0.25f;
        result = lerp(cMid1, cMid2, t);
    }
    else if (gray < 0.75f)
    {
        float t = (gray - 0.5f) / 0.25f;
        result = lerp(cMid2, cHot1, t);
    }
    else
    {
        float t = (gray - 0.75f) / 0.25f;
        result = lerp(cHot1, cHot2, t);
    }

    return result;
}

// Night vision: green, boosted contrast, vignette, noise
float3 ApplyNightVision(float3 color, float2 uv)
{
    // Desaturate
    float gray = dot(color, float3(0.299f, 0.587f, 0.114f));
    float3 base = gray.xxx;

    // Boost contrast
    base = (base - 0.5f) * 1.6f + 0.6f;

    // Tint green
    base *= float3(0.2f, 1.0f, 0.2f) * nightIntensity;

    // Vignette & noise for goggles feel
    base = ApplyVignette(base, uv);
    base = ApplyNoise(base, uv);

    return base;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.texcoord;

    float4 src = gSceneColor.Sample(gSampler, uv);
    float3 color = src.rgb;

    if (mode == 1.0f)
    {
        // Thermal
        color = ApplyThermal(color);
        color = ApplyNoise(color, uv);
    }
    else if (mode == 2.0f)
    {
        // Night vision
        color = ApplyNightVision(color, uv);
    }
    else
    {
        // Pass-through
        color = src.rgb;
    }

    return float4(saturate(color), src.a);
}
