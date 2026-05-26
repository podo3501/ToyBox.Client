cbuffer FrameCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 world;
};

struct GridVertex
{
    float3 position;
    float3 color;
};

StructuredBuffer<GridVertex> VertexBuffer : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 color    : COLOR;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    GridVertex v = VertexBuffer[vID];

    float4 worldPos = mul(float4(v.position, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, proj);

    output.color = v.color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}