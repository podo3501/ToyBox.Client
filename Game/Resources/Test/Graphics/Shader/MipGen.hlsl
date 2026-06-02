struct MipInfo
{
    uint srcMipIndex;
    uint dstMipIndex;
    uint2 DstSize; // 4 DWORDS 유지
};

ConstantBuffer<MipInfo> g_mipInfo : register(b0);

float3 LinearToSRGB(float3 c) { return pow(c, 1.0f / 2.2f); }

[numthreads(8, 8, 1)]
void CSMain(uint2 id : SV_DispatchThreadID)
{
    if (id.x >= g_mipInfo.DstSize.x || id.y >= g_mipInfo.DstSize.y)
        return;

    Texture2D<float4> srcMip = ResourceDescriptorHeap[g_mipInfo.srcMipIndex];
    RWTexture2D<float4> dstMip = (RWTexture2D<float4>)ResourceDescriptorHeap[g_mipInfo.dstMipIndex];

    uint2 srcCoord = id * 2;

    float4 color00 = srcMip.Load(uint3(srcCoord, 0));
    float4 color10 = srcMip.Load(uint3(srcCoord + uint2(1, 0), 0));
    float4 color01 = srcMip.Load(uint3(srcCoord + uint2(0, 1), 0));
    float4 color11 = srcMip.Load(uint3(srcCoord + uint2(1, 1), 0));

#if IS_DATA_MAP
    dstMip[id] = (color00 + color10 + color01 + color11) * 0.25f;
#else
    float3 finalLinearRGB = (color00.rgb + color10.rgb + color01.rgb + color11.rgb) * 0.25f;
    float  finalAlpha     = (color00.a + color10.a + color01.a + color11.a) * 0.25f;

    dstMip[id] = float4(LinearToSRGB(finalLinearRGB), finalAlpha); // UAV에 저장할 때는 하드웨어 지원이 안 되므로 수동 변환 유지
#endif
}