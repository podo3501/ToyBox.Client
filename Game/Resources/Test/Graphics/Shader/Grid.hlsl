struct GridVertex
{
    float3 position;
    float4 color;
};

cbuffer GridIndicesCB : register(b0)
{
    uint g_vbIndex;
};

cbuffer FrameCB : register(b1)
{
    float4x4 view;
    float4x4 proj;
};

cbuffer ObjectCB : register(b2)
{
    float4x4 world;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    StructuredBuffer<GridVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    GridVertex v = vb[vID];

    float4 worldPos = mul(float4(v.position, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    float4 clipPos = mul(viewPos, proj);

    output.pos = clipPos;
    output.color = v.color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}