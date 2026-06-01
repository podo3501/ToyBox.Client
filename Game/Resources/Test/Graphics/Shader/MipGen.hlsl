struct MipInfo
{
    uint srcMipIndex;
    uint dstMipIndex;
    uint2 DstSize; //출력밉의 가로, 세로 해상도
};

ConstantBuffer<MipInfo> g_mipInfo : register(b0);
SamplerState samplerLinearClamp : register(s0);

[numthreads(8, 8, 1)]
void CSMain(uint2 id : SV_DispatchThreadID)
{
    if (id.x >= g_mipInfo.DstSize.x || id.y >= g_mipInfo.DstSize.y)
        return;

    uint2 srcSize = g_mipInfo.DstSize * 2;
    float2 srcUV = (id * 2 + 0.5f) / (float2)srcSize;
    
    Texture2D<float4> srcMip = ResourceDescriptorHeap[g_mipInfo.srcMipIndex];
    float4 color = srcMip.SampleLevel(samplerLinearClamp, srcUV, 0);

    RWTexture2D<float4> dstMip = (RWTexture2D<float4>)ResourceDescriptorHeap[g_mipInfo.dstMipIndex];
    dstMip[id] = color;
}