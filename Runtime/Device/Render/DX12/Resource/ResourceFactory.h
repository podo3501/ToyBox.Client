#pragma once
#include "Resource.h"

class Device;

class ResourceFactory
{
public:
    ~ResourceFactory();
    ResourceFactory(Device& device);

    Resource CreateUploadResource(size_t size);
    Resource CreateTextureResource(const D3D12_RESOURCE_DESC& desc);
    Resource CreateBufferResource(UINT64 size);
    Resource CreateShadowResource(UINT width, UINT height);

    UINT64 GetRequiredIntermediateSize(
        const D3D12_RESOURCE_DESC& desc,
        UINT firstSubresource,
        UINT numSubresources,
        UINT64 offset);

private:
    Device& m_device;
};
