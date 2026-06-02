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
    DescriptorFactory(ID3D12Device* device, DescriptorAllocator* srvAllocator);
    bool CreateTextureViews(TextureResource* texRes, bool generateMips);
    UINT CreateBufferSRV(ID3D12Resource* buffer, UINT elementCount, UINT elementStride);
    DescriptorAllocator* GetDescriptorAllocator() { return m_srvAllocator; }

private:
    UINT CreateTextureSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevels);
    UINT CreateMipSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateMipUAV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel);
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(UINT numElements, UINT stride) const;

    ID3D12Device* m_device{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };
};
