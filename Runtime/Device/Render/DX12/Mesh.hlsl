cbuffer ObjectCB : register(b0)
{
    float4x4 world;
};

cbuffer FrameCB : register(b1)
{
    float4x4 view;
    float4x4 proj;
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VSInput
{
    float3 pos     : POSITION;
    float3 normal  : NORMAL;
    float2 uv      : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos  = mul(worldPos, view);
    float4 clipPos  = mul(viewPos, proj);

    output.pos = clipPos;
    output.uv  = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = gTexture.Sample(gSampler, input.uv);
    return color;
}