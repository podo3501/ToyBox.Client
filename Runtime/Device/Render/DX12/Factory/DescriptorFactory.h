#pragma once
#include "d3dx12.h"

struct DescriptorConfig;
class Device;
class Resource;
class TextureResource;
class DescriptorAllocator;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    explicit DescriptorFactory(Device& device);
    bool Initialize(const DescriptorConfig& config);
    UINT CreateBufferSRV(const Resource& resBuffer, UINT elementCount, UINT elementStride); //vb, ib 만들때
    UINT CreateTextureSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels = 1);
    UINT CreateTextureDSV(const Resource& res, DXGI_FORMAT format, UINT mipSlice = 0);
    bool CreateTextureViews(TextureResource* texRes, bool generateMips);
    DescriptorAllocator* GetSrvAllocator() { return m_srvAllocator.get(); }
    DescriptorAllocator* GetDsvAllocator() { return m_dsvAllocator.get(); }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(UINT index); //dsv는 gpu 핸들 api를 제공하지 않는다.
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle(UINT index);

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(UINT numElements, UINT stride) const;
    UINT CreateMipSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateMipUAV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);

    Device& m_device;
    unique_ptr<DescriptorAllocator> m_srvAllocator;
    unique_ptr<DescriptorAllocator> m_dsvAllocator;
};
