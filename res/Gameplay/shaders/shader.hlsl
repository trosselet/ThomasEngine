struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};


struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VSOutput vsmain(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 1.0f);
    output.color = input.color;
    return output;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    return input.color;
}
