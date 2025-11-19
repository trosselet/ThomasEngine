Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct VSIn
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};


struct PSIn
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};


PSIn vsmain(VSIn input)
{
    PSIn o;
    o.pos = float4(input.pos, 1.0f);
    o.uv = input.uv;
    return o;
}

float4 psmain(PSIn i) : SV_TARGET
{
    return tex.Sample(samp, i.uv);
}