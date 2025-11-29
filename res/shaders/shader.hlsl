Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);


cbuffer cbPass : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

VSOutput vsmain(VSInput vin)
{
    VSOutput vout;
    vout.pos = mul(mul(mul(float4(vin.pos, 1.0f), gWorld), gView), gProj);
    vout.color = vin.color;
    vout.texcoord = vin.uv;
    vout.normal = vin.normal;
    
    return vout;
}


float4 psmain(VSOutput input) : SV_TARGET
{
    return gTexture.Sample(gSampler, input.texcoord) * input.color;

}
