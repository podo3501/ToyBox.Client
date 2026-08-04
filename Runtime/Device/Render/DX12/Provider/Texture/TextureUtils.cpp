#include "pch.h"
#include "TextureUtils.h"
#include "Resource/Resource.h"
#include "Helpers/TextureHelpers.h"
#include "Command/CommandList.h"
#include "Core/D3D12Conversions.h"
#include "GameClient/Asset/TextureAsset.h"
#include "GameClient/Asset/TextureCubeAsset.h"

static bool IsUAVCompatibleFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return true;
    default: return false;
    }
}

static void ApplyMipSettings(D3D12_RESOURCE_DESC& desc, bool canGenerateMips)
{
    if (!canGenerateMips) return;

    desc.MipLevels = 0; // full mip chain
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
}

D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips)
{
    auto texDesc = CreateTextureDescriptor(asset.size.width, asset.size.height, ToDXGIFormat(asset.format));
    ApplyMipSettings(texDesc, mips);

    return texDesc;
}


D3D12_RESOURCE_DESC CreateTextureCubeDesc(const TextureCubeAsset& asset)
{
    auto texDesc = CreateTextureDescriptor(asset.width, asset.height, ToDXGIFormat(asset.format));
    texDesc.DepthOrArraySize = 6; // 큐브맵 = 6면 배열
    texDesc.MipLevels = static_cast<UINT16>(asset.mipCount); // cmgen이 이미 구운 밉체인 그대로 사용 (0=full chain 아님)
    // UAV 플래그 불필요 - 런타임 밉 생성 없음
    return texDesc;
}

bool CanGenerateMips(const TextureAsset& asset, bool generateMips)
{
    DXGI_FORMAT format = ToDXGIFormat(asset.format);
    return generateMips && IsUAVCompatibleFormat(format);
}

void UploadTexture(
    CommandList& uploadCmd,
    const TextureAsset& asset,
    Resource& texRes,
    Resource& uploadRes,
    UINT64 offset)
{
    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = asset.pixels.data();
    subresource.RowPitch = asset.stride;
    subresource.SlicePitch = asset.stride * asset.size.height;

    UpdateSubresources(
        uploadCmd.Get(),
        texRes.Get(),
        uploadRes.Get(),
        offset,
        0,
        1,
        &subresource
    );
}

void UploadTextureCube(
    CommandList& uploadCmd,
    const TextureCubeAsset& asset,
    Resource& texRes,
    Resource& uploadRes,
    UINT64 offset)
{
    const UINT subresourceCount = asset.mipCount * asset.faceCount;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    subresources.reserve(subresourceCount);

    // D3D12 서브리소스 순서 규칙: mip이 가장 안쪽, 그다음 face(array slice)
    // subresourceIndex = mip + face * mipCount  (기존 asset.subImages 인덱싱과 동일한 규칙)
    for (uint32_t face = 0; face < asset.faceCount; ++face)
    {
        for (uint32_t mip = 0; mip < asset.mipCount; ++mip)
        {
            const auto& sub = asset.subImages[mip + face * asset.mipCount];

            D3D12_SUBRESOURCE_DATA subresource{};
            subresource.pData = sub.pixels.data();
            subresource.RowPitch = sub.rowPitch;
            subresource.SlicePitch = static_cast<LONG_PTR>(sub.pixels.size());

            subresources.push_back(subresource);
        }
    }

    UpdateSubresources(
        uploadCmd.Get(),
        texRes.Get(),
        uploadRes.Get(),
        offset,
        0,
        subresourceCount,
        subresources.data()
    );
}