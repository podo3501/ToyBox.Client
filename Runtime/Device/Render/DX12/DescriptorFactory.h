#pragma once
#include "DescriptorAllocation.h"
#include "d3dx12.h"

struct TextureDesc;

using Microsoft::WRL::ComPtr;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    DescriptorFactory(ID3D12Device* device, DescriptorAllocator* srvAllocator);
    DescriptorAllocation CreateSRV(ID3D12Resource* res, const TextureDesc& desc, bool generateMips);

private:
    ID3D12Device* m_device{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };
};
