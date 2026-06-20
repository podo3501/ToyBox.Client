#include "pch.h"
#include "TextureHelpers.h"

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