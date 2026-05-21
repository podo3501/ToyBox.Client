cbuffer UIFrameCB : register(b1)
{
    float4x4 world;
    float4x4 projection;
};

struct UIVertex
{
    float3 pos;
    float4 color;
    float2 uv;
};

StructuredBuffer<UIVertex> VertexBuffer : register(t0);
StructuredBuffer<uint> IndexBuffer : register(t1);

Texture2D tex : register(t2);
SamplerState samp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    uint vertexIndex = IndexBuffer[vID];
    UIVertex input = VertexBuffer[vertexIndex];

    float4 localPos = float4(input.pos, 1.0f);
    float4 worldPos = mul(localPos, world);
    float4 clipPos = mul(worldPos, projection);

    output.pos = clipPos;
    //output.pos = worldPos;
    output.uv = input.uv;
    output.color = input.color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.uv);
    return texColor * input.color; // 색 곱해서 tint 가능
}