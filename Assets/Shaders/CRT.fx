// CRT.fx
// Post-process CRT shader for screen quad (VertexPX)

Texture2D gSceneColor : register(t0);
SamplerState gSampler : register(s0);

cbuffer CRTSettings : register(b0)
{
    float distortionStrength; // 0..0.6
    float scanlineIntensity; // 0..1
    float noiseIntensity; // 0..0.5
    float vignetteStrength; // 0..3

    float time; // seconds
    float chromaOffset; // chroma aberr scale
    float brightness; // 0.5..2
    float contrast; // 0.5..2

    float2 screenSize; // width, height
    float2 padding;
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
    // Screen quad already in clip space [-1,1]
    output.position = float4(input.position, 1.0f);
    output.texcoord = input.texcoord;
    return output;
}

// ---------- Helpers ----------

float Hash(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453);
}

float3 ApplyVignette(float3 color, float2 uv)
{
    if (vignetteStrength <= 0.0f)
        return color;

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
    float3 noise = (n - 0.5f).xxx; // centered
    return lerp(color, color + noise, noiseIntensity);
}

float3 ApplyScanlines(float3 color, float2 uv)
{
    if (scanlineIntensity <= 0.0f)
        return color;

    float row = uv.y * screenSize.y;
    float s = 0.5f + 0.5f * sin(row * 3.14159f);
    float factor = lerp(1.0f - scanlineIntensity, 1.0f, s);
    return color * factor;
}

// Very simple RGB mask: alternating stripes
float3 ApplyMask(float3 color, float2 uv)
{
    float stripe = frac(uv.x * screenSize.x * 0.5f); // 2 pixels per cycle
    float3 mask = (stripe < 0.33f) ? float3(1.0f, 0.9f, 0.9f) :
                  (stripe < 0.66f) ? float3(0.9f, 1.0f, 0.9f) :
                                     float3(0.9f, 0.9f, 1.0f);
    return color * mask;
}

// ---------- Main PS ----------

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.texcoord;

    // Barrel distortion
    float2 centered = uv * 2.0f - 1.0f; // [-1,1]
    float r2 = dot(centered, centered);
    float k = distortionStrength;
    float2 warped = centered * (1.0f + k * r2);
    float2 warpedUV = warped * 0.5f + 0.5f;

    // Outside screen -> black border
    if (warpedUV.x < 0.0f || warpedUV.x > 1.0f ||
        warpedUV.y < 0.0f || warpedUV.y > 1.0f)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // Chromatic aberration (tiny shift of R/B)
    float caScale = chromaOffset * 0.003f;
    float2 caDir = centered * caScale;

    float3 cR = gSceneColor.Sample(gSampler, warpedUV + caDir).rgb;
    float3 cG = gSceneColor.Sample(gSampler, warpedUV).rgb;
    float3 cB = gSceneColor.Sample(gSampler, warpedUV - caDir).rgb;

    float3 color = float3(cR.r, cG.g, cB.b);

    // Scanlines + mask + vignette + noise
    color = ApplyScanlines(color, warpedUV);
    color = ApplyMask(color, warpedUV);
    color = ApplyVignette(color, warpedUV);
    color = ApplyNoise(color, warpedUV);

    // Brightness / contrast
    color = (color - 0.5f) * contrast + 0.5f;
    color *= brightness;

    return float4(saturate(color), 1.0f);
}
