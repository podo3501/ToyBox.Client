#pragma once
#include "AssetData.h"
#include "TextureTypes.h"

struct CubemapMipFace
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t rowPitch{ 0 }; // byte
    std::vector<uint8_t> pixels;
};

struct CubemapAsset : public AssetData
{
    CORE_DECLARE_TYPE(CubemapAsset)

    PixelFormat format{ PixelFormat::Unknown };
    ColorSpace colorSpace{ ColorSpace::Linear }; // HDR 환경맵은 보통 리니어
    uint32_t width{ 0 }; // mip0 기준
    uint32_t height{ 0 };
    uint32_t mipCount{ 0 };
    uint32_t faceCount{ 6 };

    std::vector<CubemapMipFace> subImages; // index = mip + face * mipCount  (D3D12 subresource 인덱싱 규칙과 동일하게 맞춰둠 -> 나중에 업로드할 때 편함)
};