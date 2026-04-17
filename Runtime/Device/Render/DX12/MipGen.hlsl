Texture2D SrcMip : register(t0);
RWTexture2D<float4> DstMip : register(u0);

cbuffer MipInfo : register(b0)
{
    uint2 DstSize;
}

SamplerState samplerLinearClamp : register(s0);

[numthreads(8,8,1)]
void main(uint2 id : SV_DispatchThreadID)
{
    if (id.x >= DstSize.x || id.y >= DstSize.y)
        return;

    uint2 srcSize = DstSize * 2;
    float2 srcUV = (id * 2 + 0.5f) / (float2)srcSize;
    float4 color = SrcMip.SampleLevel(samplerLinearClamp, srcUV, 0);

    DstMip[id] = color;
}