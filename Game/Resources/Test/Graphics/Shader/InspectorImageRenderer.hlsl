static const float2 Pos[6] =
{
    float2(0, 0),
    float2(1, 0),
    float2(1, 1),

    float2(0, 0),
    float2(1, 1),
    float2(0, 1)
};

static const float2 UV[6] =
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),

    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};

cbuffer InspectorTextureCB : register(b0)
{
    uint g_srvIndex;
};

cbuffer InspectorDrawCB : register(b1)
{
    float4x4 world;
    float4x4 projection;

    float2 imageSize;
    float2 padding;
};

SamplerState samp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    float4 localPos = float4(Pos[vID], 0.0f, 1.0f);

    float4 worldPos = mul(localPos, world);
    output.pos = mul(worldPos, projection);

    output.uv = UV[vID];

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    Texture2D tex = ResourceDescriptorHeap[g_srvIndex];

    // 화면 기준 1픽셀
    float2 border = 1.0 / imageSize;

    if (input.uv.x < border.x ||
        input.uv.x > 1.0 - border.x ||
        input.uv.y < border.y ||
        input.uv.y > 1.0 - border.y)
    {
       return float4(0.4, 0.4, 0.4, 1.0);
    }

    float2 uv = (input.uv - border) / (1.0 - border * 2.0);

    return tex.Sample(samp, uv);
}

