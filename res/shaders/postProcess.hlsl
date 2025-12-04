struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOut vsmain(uint vertexID : SV_VertexID)
{
    VSOut output;
    
    float2 positions[3] = { float2(-1, -1), float2(-1, 3), float2(3, -1) };
    output.pos = float4(positions[vertexID], 0, 1);
    
    output.uv = positions[vertexID] * 0.5 + 0.5;
    output.uv.y = 1.0 - output.uv.y;

    return output;
}

Texture2D inputTexture : register(t0);
SamplerState linearSampler : register(s0);

float4 psmain(VSOut input) : SV_TARGET
{
    float4 color = inputTexture.Sample(linearSampler, input.uv);
    float4 redFilter = float4(1, 0, 0, 0.0);
    return lerp(color, redFilter, redFilter.a);
}
