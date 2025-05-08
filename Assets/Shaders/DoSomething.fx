// simple shader that takes a position and a color 

// defines expected vertex data per vertex
struct VS_INPUT
{
    float3 position : POSITION;
};

// output that gets interpolated in rasterizer
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

// gets applied to every vertex
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position=float4(input.position, 1.0f);
    return output;
}

// lerped data that is converted to a pixel on screen
float4 PS(VS_OUTPUT input) : SV_Target
{
    // output color
    return float4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
}