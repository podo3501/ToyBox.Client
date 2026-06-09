#pragma once
#include "../d3dx12.h"

struct TextureDesc;
class TextureResource;
class DescriptorAllocator;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    DescriptorFactory(ID3D12Device* device, DescriptorAllocator* srvAllocator, DescriptorAllocator* dsvAllocator);
    UINT CreateBufferSRV(ID3D12Resource* buffer, UINT elementCount, UINT elementStride); //vb, ib ¸¸µé¶§
    UINT CreateTextureSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevels = 1);
    UINT CreateTextureDSV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipSlice = 0);
    bool CreateTextureViews(TextureResource* texRes, bool generateMips);
    DescriptorAllocator* GetSrvAllocator() { return m_srvAllocator; }
    DescriptorAllocator* GetDsvAllocator() { return m_dsvAllocator; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(UINT index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle(UINT index);

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(UINT numElements, UINT stride) const;
    UINT CreateMipSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateMipUAV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel);

    ID3D12Device* m_device{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };
    DescriptorAllocator* m_dsvAllocator{ nullptr };
};
