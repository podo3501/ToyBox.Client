#include "pch.h"
#include "ResourceFactory.h"
#include "Core/Device.h"

static D3D12_RESOURCE_DESC CreateBufferDesc(UINT64 size) { return CD3DX12_RESOURCE_DESC::Buffer(size); }

ResourceFactory::~ResourceFactory() = default;
ResourceFactory::ResourceFactory(Device& device) :
    m_device{ device }
{}

Resource ResourceFactory::CreateTextureResource(const D3D12_RESOURCE_DESC& desc)
{
    return m_device.CreateResource(desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON);
}

Resource ResourceFactory::CreateResource(UINT64 size, ResInitType type)
{
    const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

    D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON;

    switch (type)
    {
    case ResInitType::Upload: //cpu->gpu
        heapType = D3D12_HEAP_TYPE_UPLOAD;
        initState = D3D12_RESOURCE_STATE_GENERIC_READ;
        break;

    case ResInitType::Default:
        heapType = D3D12_HEAP_TYPE_DEFAULT;
        initState = D3D12_RESOURCE_STATE_COMMON;
        break;

    case ResInitType::Readback: //gpu->cpu
        heapType = D3D12_HEAP_TYPE_READBACK;
        initState = D3D12_RESOURCE_STATE_COPY_DEST;
        break;
    }

    return m_device.CreateResource(desc, heapType, initState);
}

UINT64 ResourceFactory::GetRequiredIntermediateSize(
    const D3D12_RESOURCE_DESC& desc,
    UINT firstSubresource,
    UINT numSubresources,
    UINT64 offset)
{
    return m_device.GetRequiredIntermediateSize(desc, firstSubresource, numSubresources, offset);
}
