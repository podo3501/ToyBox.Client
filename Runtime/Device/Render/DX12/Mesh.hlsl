cbuffer ObjectCB : register(b0)
{
    float4x4 world;
};

cbuffer FrameCB : register(b1)
{
    float4x4 view;
    float4x4 proj;
};

struct MeshVertex
{
    float3 pos;
    float3 normal;
    float2 uv;
};

StructuredBuffer<MeshVertex> VertexBuffer : register(t0);
StructuredBuffer<uint>   IndexBuffer  : register(t1);

Texture2D gTexture : register(t2);
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    uint vertexIndex = IndexBuffer[vID];
    MeshVertex input = VertexBuffer[vertexIndex];

    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos  = mul(worldPos, view);
    float4 clipPos  = mul(viewPos, proj);

    output.pos = clipPos;
    output.uv  = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
//    float4 color = gTexture.Sample(gSampler, input.uv);
//    return color;
     return float4(1,1,1,1);
}