#pragma once
#include "Resource/Resource.h"

class Device;

class ResourceFactory
{
public:
    ~ResourceFactory();
    ResourceFactory(Device& device);

    Resource CreateUploadResource(UINT64 size); //cpu->gpu
    Resource CreateTextureResource(const D3D12_RESOURCE_DESC& desc);
    Resource CreateBufferResource(UINT64 size);
    Resource CreateReadbackBuffer(UINT64 size); //gpu->cpu
    Resource CreateShadowResource(UINT width, UINT height);

    UINT64 GetRequiredIntermediateSize(
        const D3D12_RESOURCE_DESC& desc,
        UINT firstSubresource,
        UINT numSubresources,
        UINT64 offset);

private:
    Device& m_device;
};
