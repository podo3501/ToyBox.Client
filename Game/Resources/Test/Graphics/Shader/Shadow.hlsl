struct MeshVertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
};

cbuffer MeshIndicesCB : register(b0)
{
    uint g_vbIndex;
    uint g_ibIndex;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 world;
};

cbuffer ShadowFrameCB : register(b2)
{
    float4x4 lightViewProj;
};

struct VSOutput
{
    float4 pos : SV_POSITION; // 조명 공간의 Clip Space 좌표가 담깁니다.
};

VSOutput VSMain(uint vID : SV_VertexID)
{
    VSOutput output;

    StructuredBuffer<uint> ib = ResourceDescriptorHeap[g_ibIndex];
    uint vertexIndex = ib[vID];

    StructuredBuffer<MeshVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    float3 inputPos = vb[vertexIndex].pos;

    float4 worldPos = mul(float4(inputPos, 1.0f), world);
    output.pos = mul(worldPos, lightViewProj);

    return output;
}

void PSMain(VSOutput input)
{
    // 아무것도 하지 않고 리턴 (하드웨어가 SV_POSITION의 Z/W 값을 깊이 버퍼에 자동 기록)
}