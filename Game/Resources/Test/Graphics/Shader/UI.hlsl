struct UIVertex
{
    float3 pos;
    float4 color;
    float2 uv;
};

SamplerState samp : register(s0);

cbuffer UIIndicesCB : register(b0)
{
    uint g_vbIndex;
    uint g_ibIndex;
    uint g_textureIndex;
};

cbuffer UIDrawCB : register(b1)
{
    float4x4 world;
    float4x4 projection;
    float4 uvTransform; // x=u0, y=v0, z=u1, w=v1
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    StructuredBuffer<uint> ib = ResourceDescriptorHeap[g_ibIndex];
    uint vertexIndex = ib[vID];

    StructuredBuffer<UIVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    UIVertex input = vb[vertexIndex];

    float4 localPos = float4(input.pos, 1.0f);
    float4 worldPos = mul(localPos, world);
    float4 clipPos = mul(worldPos, projection);

    output.pos = clipPos;
    output.color = input.color;
    output.uv.x = lerp(uvTransform.x, uvTransform.z, input.uv.x);
    output.uv.y = lerp(uvTransform.y, uvTransform.w, input.uv.y);

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    Texture2D uiTex = ResourceDescriptorHeap[g_textureIndex];
    float4 texColor = uiTex.Sample(samp, input.uv);

    return texColor * input.color; // 색 곱해서 tint 가능
}