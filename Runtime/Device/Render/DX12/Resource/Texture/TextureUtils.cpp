#include "pch.h"
#include "TextureUtils.h"
#include "Resource/Resource.h"
#include "Command/CommandList.h"
#include "Core/D3D12Conversions.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "d3dx12.h"

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

D3D12_RESOURCE_DESC CreateTextureDescriptor(
    UINT64 width,
    UINT height,
    DXGI_FORMAT format)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.MipLevels = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    Assert(desc.Flags == 0);

    return desc;
}

D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips)
{
    auto texDesc = CreateTextureDescriptor(asset.width, asset.height, ToDXGIFormat(asset.format));
    ApplyMipSettings(texDesc, mips);

    return texDesc;
}

bool ShouldGenerateMips(const TextureAsset& asset, bool generateMips)
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
    subresource.SlicePitch = asset.stride * asset.height;

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