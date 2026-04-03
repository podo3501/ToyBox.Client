#include "pch.h"
#include "ResourceUploader.h"
#include "CommandScheduler.h"
#include "ImageData.h"

ResourceUploader::~ResourceUploader() = default;
ResourceUploader::ResourceUploader(ID3D12Device* device) :
    m_device{ device }
{}

static D3D12_RESOURCE_DESC CreateTexture2DDesc(const ImageData& img)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = img.width;
    desc.Height = img.height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    return desc;
}

static D3D12_RESOURCE_DESC CreateBufferDesc(UINT64 size)
{
    return CD3DX12_RESOURCE_DESC::Buffer(size);
}

static void UploadSubresource(
    ID3D12GraphicsCommandList* cmd,
    ID3D12Resource* dest,
    ID3D12Resource* upload,
    const D3D12_SUBRESOURCE_DATA& subresource)
{
    UpdateSubresources(cmd, dest, upload, 0, 0, 1, &subresource);
}

ComPtr<ID3D12Resource> ResourceUploader::UploadTexture(
    ID3D12GraphicsCommandList* uploadCmd,
    const ImageData& img,
    ComPtr<ID3D12Resource>& outUploadBuffer)
{
    auto texDesc = CreateTexture2DDesc(img);
    auto texture = CreateResource(
        texDesc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);

    UINT64 uploadSize = 0;
    m_device->GetCopyableFootprints(&texDesc, 0, 1, 0,
        nullptr, nullptr, nullptr, &uploadSize);

    outUploadBuffer = CreateResource(
        CreateBufferDesc(uploadSize),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = img.pixels.data();
    subresource.RowPitch = img.stride;
    subresource.SlicePitch = img.stride * img.height;
    UploadSubresource(uploadCmd, texture.Get(), outUploadBuffer.Get(), subresource);

    return texture;
}

ComPtr<ID3D12Resource> ResourceUploader::UploadVertexBuffer(
    ID3D12GraphicsCommandList* cmd,
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