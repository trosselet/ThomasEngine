Texture2D gDiffuseTex : register(t0);
Texture2D gNormalTex : register(t1);
Texture2D gSpecularTex : register(t2);
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

cbuffer MaterialCB : register(b2)
{
    float4 gAmbientColor;
    float4 gDiffuseColor;
    float4 gSpecularColor;
    float gShininess;
    float gRoughness;
    float gMetallic;
    float gEmmisiveStrength;
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
    float3 worldPos : POSITION1;
};


static const float3 LightDirection = normalize(float3(-0.5f, -1.0f, -0.6f));
static const float3 LightColor = float3(1.0, 1.0, 1.0);
static const float3 AmbientLight = float3(1.0, 1.0, 1.0);

VSOutput vsmain(VSInput vin)
{
    VSOutput vout;

    float4 worldPos = mul(float4(vin.pos, 1.0f), gWorld);

    vout.pos = mul(mul(worldPos, gView), gProj);
    vout.color = vin.color;
    vout.texcoord = vin.uv;

    vout.normal = normalize(mul(float4(vin.normal, 0.0f), gWorld).xyz);
    vout.worldPos = worldPos.xyz;

    return vout;
}


float3 ComputeLighting(float3 N, float3 V, float3 baseColor, float3 specTex)
{
    float3 LightDir = normalize(float3(0.0f, -1.0f, -0.6f));

    // Diffuse
    float NdotL = saturate(dot(N, LightDir));
    float3 diffuse = gDiffuseColor.rgb * NdotL;

    // Specular
    float3 R = reflect(-LightDir, N);
    float spec = pow(saturate(dot(R, V)), gShininess);
    float3 specular = specTex * spec;

    // Ambient
    float3 ambient = gAmbientColor.rgb;

    // Combine
    float3 lighting = ambient + diffuse + specular;

    // Emissive
    lighting += gEmmisiveStrength * gDiffuseColor.rgb;

    return lighting;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float4 diffuseTex = gDiffuseTex.Sample(gSampler, input.texcoord);
    float4 specularTex = gSpecularTex.Sample(gSampler, input.texcoord);

    float3 N = normalize(input.normal);
    float3 V = normalize(-input.worldPos);

    float3 albedo = diffuseTex.rgb * input.color.rgb;

    float3 lit = ComputeLighting(N, V, diffuseTex.rgb, specularTex.rgb);
    
    return float4(lit * albedo, diffuseTex.a);
}

