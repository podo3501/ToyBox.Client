#include "pch.h"
#include "ResourceUploader.h"
#include "CommandScheduler.h"
#include "CommandList.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"

ResourceUploader::~ResourceUploader() = default;
ResourceUploader::ResourceUploader(ID3D12Device* device) :
    m_device{ device }
{}

static DXGI_FORMAT GetFormat(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::RGB8: return DXGI_FORMAT_R8G8B8A8_UNORM; //3채널은 지원하지 않는다. 일단 이걸로.
    case PixelFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    return DXGI_FORMAT_R8G8B8A8_UNORM;
}

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

static D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool generateMips)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = asset.width;
    desc.Height = asset.height;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.Format = GetFormat(asset.format);
    desc.MipLevels = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    return desc;
}

static D3D12_RESOURCE_DESC CreateBufferDesc(UINT64 size)
{
    return CD3DX12_RESOURCE_DESC::Buffer(size);
}

static void ApplyMipSettings(D3D12_RESOURCE_DESC& desc, bool canGenerateMips)
{
    if (!canGenerateMips) return;

    desc.MipLevels = 0; // full mip chain
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
}

static void UploadSubresource(
    CommandList& cmd,
    ID3D12Resource* dest,
    ID3D12Resource* upload,
    const D3D12_SUBRESOURCE_DATA& subresource)
{
    UpdateSubresources(cmd.Get(), dest, upload, 0, 0, 1, &subresource);
}

bool ResourceUploader::ShouldGenerateMips(const TextureAsset& asset, bool generateMips)
{
    DXGI_FORMAT format = GetFormat(asset.format);
    return generateMips && IsUAVCompatibleFormat(format);
}

ComPtr<ID3D12Resource> ResourceUploader::UploadTexture(
    CommandList& uploadCmd,
    const TextureAsset& asset,
    bool generateMips,
    ComPtr<ID3D12Resource>& outUploadBuffer)
{
    auto texDesc = CreateTexture2DDesc(asset, generateMips);
    ApplyMipSettings(texDesc, generateMips);
    auto texture = CreateResource(texDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);

    UINT64 uploadSize = 0;
    UINT numSubresources = 1; // only mip0 uploaded
    m_device->GetCopyableFootprints(&texDesc, 0, numSubresources, 0,
        nullptr, nullptr, nullptr, &uploadSize);

    outUploadBuffer = CreateResource(
        CreateBufferDesc(uploadSize),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = asset.pixels.data();
    subresource.RowPitch = asset.stride;
    subresource.SlicePitch = asset.stride * asset.height;
    UploadSubresource(uploadCmd, texture.Get(), outUploadBuffer.Get(), subresource);

    return texture;
}

ComPtr<ID3D12Resource> ResourceUploader::UploadVertexBuffer(
    CommandList& cmd,
    const void* data,
    UINT bufferSize,
    ComPtr<ID3D12Resource>& outUploadBuffer)
{
    auto vertexBuffer = CreateResource(
        CreateBufferDesc(bufferSize),
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);

    outUploadBuffer = CreateResource(
        CreateBufferDesc(bufferSize),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = data;
    subresource.RowPitch = bufferSize;
    subresource.SlicePitch = bufferSize;
    UploadSubresource(cmd, vertexBuffer.Get(), outUploadBuffer.Get(), subresource);

    return vertexBuffer;
}

ComPtr<ID3D12Resource> ResourceUploader::CreateResource(
    const D3D12_RESOURCE_DESC& desc,
    D3D12_HEAP_TYPE heapType,
    D3D12_RESOURCE_STATES state)
{
    CD3DX12_HEAP_PROPERTIES heap(heapType);
    
    ComPtr<ID3D12Resource> res;
    m_device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        state,
        nullptr,
        IID_PPV_ARGS(&res));

    return res;
}