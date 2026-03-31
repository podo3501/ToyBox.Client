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
    PSInput o;
    o.pos = float4(input.pos, 1);
    o.color = input.color;
    o.uv = input.uv;
    return o;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.uv);
    return texColor * input.color; // 색 곱해서 tint 가능
//return tex.Sample(samp, float2(0.5, 0.5));
}