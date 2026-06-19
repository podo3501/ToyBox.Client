#include "pch.h"
#include "ResourceFactory.h"
#include "Core/Device.h"
#include "Provider/Texture/TextureUtils.h"

static D3D12_RESOURCE_DESC CreateBufferDesc(UINT64 size) { return CD3DX12_RESOURCE_DESC::Buffer(size); }

ResourceFactory::~ResourceFactory() = default;
ResourceFactory::ResourceFactory(Device& device) :
    m_device{ device }
{}

Resource ResourceFactory::CreateUploadResource(UINT64 size)
{
    return m_device.CreateResource(
        CreateBufferDesc(size),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

Resource ResourceFactory::CreateTextureResource(const D3D12_RESOURCE_DESC& desc)
{
    return m_device.CreateResource(desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);
}

Resource ResourceFactory::CreateBufferResource(UINT64 size)
{
    return m_device.CreateResource(
        CreateBufferDesc(size),
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);
}

Resource ResourceFactory::CreateReadbackBuffer(UINT64 size)
{
    return m_device.CreateResource(
        CreateBufferDesc(size),
        D3D12_HEAP_TYPE_READBACK,
        D3D12_RESOURCE_STATE_COPY_DEST);
}

UINT64 ResourceFactory::GetRequiredIntermediateSize(
    const D3D12_RESOURCE_DESC& desc,
    UINT firstSubresource,
    UINT numSubresources,
    UINT64 offset)
{
    return m_device.GetRequiredIntermediateSize(desc, firstSubresource, numSubresources, offset);
}
