// simple shader that takes a position and sets the color

// defines expected vertex data per vertex
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// output that gets interpolated in rasterizer
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// gets applied to every vertex
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}

// lerped data that is converted to a pixel on screen
float4 PS(VS_OUTPUT input) : SV_Target
{
    // output color
    return input.color;
}