#pragma once
#include "DescriptorAllocation.h"
#include "../d3dx12.h"

struct TextureDesc;

using Microsoft::WRL::ComPtr;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    DescriptorFactory(ID3D12Device* device, DescriptorAllocator* srvAllocator);
    DescriptorAllocation CreateTextureSRV(ID3D12Resource* res, const TextureDesc& desc, bool generateMips);
    DescriptorAllocation CreateMeshTable(
        ID3D12Resource* vb, UINT vertexCount, UINT vertexStride,
        ID3D12Resource* ib, UINT indexCount, UINT indexStride);

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(UINT numElements, UINT stride) const;

    ID3D12Device* m_device{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };
};
