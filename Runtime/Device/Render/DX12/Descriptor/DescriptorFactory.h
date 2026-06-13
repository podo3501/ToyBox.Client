#pragma once
#include "../d3dx12.h"

class Device;
class Resource;
class TextureResource;
class DescriptorAllocator;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    DescriptorFactory(Device& device, DescriptorAllocator* srvAllocator, DescriptorAllocator* dsvAllocator);
    UINT CreateBufferSRV(const Resource& resBuffer, UINT elementCount, UINT elementStride); //vb, ib ¸¸µé¶§
    UINT CreateTextureSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels = 1);
    UINT CreateTextureDSV(const Resource& res, DXGI_FORMAT format, UINT mipSlice = 0);
    bool CreateTextureViews(TextureResource* texRes, bool generateMips);
    DescriptorAllocator* GetSrvAllocator() { return m_srvAllocator; }
    DescriptorAllocator* GetDsvAllocator() { return m_dsvAllocator; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(UINT index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle(UINT index);

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(UINT numElements, UINT stride) const;
    UINT CreateMipSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateMipUAV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);

    Device& m_device;
    DescriptorAllocator* m_srvAllocator{ nullptr };
    DescriptorAllocator* m_dsvAllocator{ nullptr };
};
