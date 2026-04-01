cbuffer TransformBuffer : register(b0)
{
    float2 scale;
    float2 offset;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float2 pos = input.pos.xy;

    pos *= scale;
    pos += offset;

    output.pos = float4(pos, 0.0f, 1.0f);
    output.uv = input.uv;
    output.color = input.color;

    return output;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.uv);
    return texColor * input.color; // 색 곱해서 tint 가능
}