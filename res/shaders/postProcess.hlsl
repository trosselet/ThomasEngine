struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOut vsmain(uint vertexID : SV_VertexID)
{
    VSOut o;

    // Full-screen triangle positions
    const float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };

    // UVs
    const float2 uvs[3] =
    {
        float2(0.0, 0.0),
        float2(0.0, 1.0),
        float2(1.0, 0.0)
    };

    o.pos = float4(positions[vertexID], 0.0, 1.0);
    o.uv = uvs[vertexID];

    return o;
}


Texture2D inputTexture : register(t0);
SamplerState linearSampler : register(s0);

float4 psmain(VSOut i) : SV_TARGET
{
    //return inputTexture.Sample(linearSampler, i.uv);
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}
