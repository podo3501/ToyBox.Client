#include "pch.h"
#include "ResourceLoader.h"
#include "Command/CommandScheduler.h"
#include "Command/CommandList.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"

ResourceLoader::~ResourceLoader() = default;
ResourceLoader::ResourceLoader(ID3D12Device* device) :
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

static D3D12_RESOURCE_DESC CreateTextureDescriptor(
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

ComPtr<ID3D12Resource> ResourceLoader::CreateUploadResource(size_t size)
{
    return CreateResource(
        CreateBufferDesc(size),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

ComPtr<ID3D12Resource> ResourceLoader::CreateTextureResource(const D3D12_RESOURCE_DESC& desc)
{
    return CreateResource(desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);
}

ComPtr<ID3D12Resource> ResourceLoader::CreateBufferResource(UINT64 size)
{
    return CreateResource(
        CreateBufferDesc(size),
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);
}

ComPtr<ID3D12Resource> ResourceLoader::CreateShadowResource(UINT width, UINT height)
{
    auto desc = CreateTextureDescriptor(width, height, DXGI_FORMAT_R32_TYPELESS);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    return CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue);
}

bool ResourceLoader::ShouldGenerateMips(const TextureAsset& asset, bool generateMips)
{
    DXGI_FORMAT format = GetFormat(asset.format);
    return generateMips && IsUAVCompatibleFormat(format);
}

D3D12_RESOURCE_DESC ResourceLoader::CreateTexture2DDesc(const TextureAsset& asset, bool mips)
{
    auto texDesc = CreateTextureDescriptor(asset.width, asset.height, GetFormat(asset.format));
    ApplyMipSettings(texDesc, mips);

    return texDesc;
}

UINT64 ResourceLoader::GetTextureUploadLayout(const D3D12_RESOURCE_DESC& texDesc, size_t offset)
{
    UINT64 requiredSize = 0;
    UINT numSubresources = 1; // only mip0 uploaded. 원본만 올린다 mip1 부터 mipmap 시작. 지금은 gpu가 밉맵을 만들고 있기 때문.
    m_device->GetCopyableFootprints(&texDesc, 0, numSubresources, offset,
        nullptr, nullptr, nullptr, &requiredSize);

    return requiredSize;
}

void ResourceLoader::UploadBufferRegion(
    CommandList& cmd,
    ID3D12Resource* uploadRes,
    const UploadRegion& region)
{
    uint8_t* mapped = nullptr;
    uploadRes->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
    memcpy(mapped + region.srcOffset, region.data, region.size);
    uploadRes->Unmap(0, nullptr);

    cmd->CopyBufferRegion(
        region.dstBuffer,
        0,
        uploadRes,
        region.srcOffset,
        region.size
    );
}

void ResourceLoader::UploadTexture(
    CommandList& uploadCmd,
    const TextureAsset& asset,
    ID3D12Resource* texRes,
    ID3D12Resource* uploadRes,
    UINT64 offset)
{
    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = asset.pixels.data();
    subresource.RowPitch = asset.stride;
    subresource.SlicePitch = asset.stride * asset.height;

    UpdateSubresources(
        uploadCmd.Get(),
        texRes,
        uploadRes,
        offset,
        0,
        1,
        &subresource
    );
}
//
ComPtr<ID3D12Resource> ResourceLoader::UploadVertexBuffer(
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

ComPtr<ID3D12Resource> ResourceLoader::CreateResource(
    const D3D12_RESOURCE_DESC& desc,
    D3D12_HEAP_TYPE heapType,
    D3D12_RESOURCE_STATES state,
    const D3D12_CLEAR_VALUE* clearValue)
{
    CD3DX12_HEAP_PROPERTIES heap(heapType);
    
    ComPtr<ID3D12Resource> res;
    auto result = m_device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        state,
        clearValue,
        IID_PPV_ARGS(&res));
    Assert(result == S_OK);

    return res;
}