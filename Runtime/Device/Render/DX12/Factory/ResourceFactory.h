#pragma once
#include "Resource/Resource.h"

class Device;

enum class ResInitType
{
    Upload,
    Default,
    Readback
};

class ResourceFactory
{
public:
    ~ResourceFactory();
    ResourceFactory(Device& device);

    Resource CreateTextureResource(const D3D12_RESOURCE_DESC& desc);
    Resource CreateResource(UINT64 size, ResInitType type);

    UINT64 GetRequiredIntermediateSize(
        const D3D12_RESOURCE_DESC& desc,
        UINT firstSubresource,
        UINT numSubresources,
        UINT64 offset);

private:
    Device& m_device;
};
